/*
 *  Authors: Winsten Marks, Silas Kath
 *  Program Name: Tar Heels Allocator
 *  Description: Simple Hoard-style malloc/free implementation.
 * 		Not suitable for use for large allocatoins, or 
 * 		in multi-threaded programs.
 *  	to use: 
 *  	$ export LD_PRELOAD=/path/to/th_alloc.so <your command>
 *
 *  UNC Honor Pledge: I, Silas Kath, certify that no unauthorized assistance has been received or
 *  given in the completion of this work.
 *
 *  UNC Honor Pledge: I, Winsten Marks, certify that no unauthorized assistance has been received or
 *  given in the completion of this work.
 */

/* Hard-code some system parameters */

#define SUPER_BLOCK_SIZE 4096
#define SUPER_BLOCK_MASK (~(SUPER_BLOCK_SIZE-1))
#define MIN_ALLOC 32 /* Smallest real allocation.  Round smaller mallocs up */
#define MAX_ALLOC 2048 /* Fail if anything bigger is attempted.  
		        * Challenge: handle big allocations */
#define RESERVE_SUPERBLOCK_THRESHOLD 2

#define FREE_POISON 0xab
#define ALLOC_POISON 0xcd

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

#define assert(cond) if (!(cond)) __asm__ __volatile__ ("int $3")

/* Object: One return from malloc/input to free. */
struct __attribute__((packed)) object {
  union {
    struct object *next; // For free list (when not in use)
    char * raw; // Actual data
  };
};

/* Super block bookeeping; one per superblock.  "steal" the first
 * object to store this structure
 */
struct __attribute__((packed)) superblock_bookkeeping {
  struct superblock_bookkeeping * next; // next super block
  struct object *free_list;
  // Free count in this superblock
  uint8_t free_count; // Max objects per superblock is 128-1, so a byte is sufficient
  uint8_t level;
};
  
/* Superblock: a chunk of contiguous virtual memory.
 * Subdivide into allocations of same power-of-two size. */
struct __attribute__((packed)) superblock {
  struct superblock_bookkeeping bkeep;
  void *raw;  // Actual data here
};


/* The structure for one pool of superblocks.  
 * One of these per power-of-two */
struct superblock_pool {
  struct superblock_bookkeeping *next;
  uint64_t free_objects; // Total number of free objects across all superblocks
  uint64_t whole_superblocks; // Superblocks with all entries free
};

// 10^5 -- 10^11 == 7 levels
#define LEVELS 7
static struct superblock_pool levels[LEVELS] = {{NULL, 0, 0},
						{NULL, 0, 0},
						{NULL, 0, 0},
						{NULL, 0, 0},
						{NULL, 0, 0},
						{NULL, 0, 0},
						{NULL, 0, 0}};

static inline int size2level (ssize_t size) {
  /* Converts the size in bytes to appropriate level */
  if      (size <= MIN_ALLOC)      return 0;  // 32
  else if (size <= 2 * MIN_ALLOC)  return 1;  // 64
  else if (size <= 4 * MIN_ALLOC)  return 2;  // 128
  else if (size <= 8 * MIN_ALLOC)  return 3;  // 256
  else if (size <= 16 * MIN_ALLOC) return 4;  // 512
  else if (size <= 32 * MIN_ALLOC) return 5;  // 1024
  else                             return 6;  // 2048

}

static inline
struct superblock_bookkeeping * alloc_super (int power) {

  void *page;
  struct superblock* sb;
  // 1 << (power + 5) == 2^(power+5), example: if power = 5, object size is 2^(5+5) == 1024B
  int bytes_per_object = (1 << (power + 5));
  // Free objects is the size of the page (4KB) divided by the size of the object
  int free_objects = ((SUPER_BLOCK_SIZE) / (bytes_per_object)) - 1; 
  char *cursor;

  // Allocate a page of anonymous memory
  page = mmap(0, SUPER_BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  
  sb = (struct superblock*) page;
  // The next two lines add the SB to the front of the list
  sb->bkeep.next = levels[power].next;
  levels[power].next = &sb->bkeep;

  levels[power].whole_superblocks += 1;
  sb->bkeep.level = power;
  sb->bkeep.free_list = NULL;

  // Your code here: Calculate and fill the number of free objects in this superblock
  //  Be sure to add this many objects to levels[power]->free_objects, reserving
  //  the first one for the bookkeeping.

  // Adjust counts taking in to account bookkeeping
  levels[power].free_objects += free_objects; 
  sb->bkeep.free_count = free_objects;
  
  // The following loop populates the free list with some atrocious
  // pointer math.  You should not need to change this, provided that you
  // correctly calculate free_objects.
  
  cursor = (char *) sb;
  // skip the first object
  for (cursor += bytes_per_object; free_objects--; cursor += bytes_per_object) {
    // Place the object on the free list
    struct object* tmp = (struct object *) cursor;
    tmp->next = sb->bkeep.free_list;
    sb->bkeep.free_list = tmp;
  }
  return &sb->bkeep;
}

void *malloc(size_t size) {
  struct superblock_pool *pool;
  struct superblock_bookkeeping *bkeep;
  void *rv = NULL;
  int power = size2level(size);
  int bytes_per_object = (1 << (power + 5));

  // Check that the allocation isn't too big
  if (size > MAX_ALLOC) {
    errno = -ENOMEM;
    return NULL;
  }
  
  pool = &levels[power];

  if (!pool->free_objects) {
    bkeep = alloc_super(power);
  } else
    bkeep = pool->next;

  while (bkeep != NULL) {
    if (bkeep->free_count) {
      struct object *next = bkeep->free_list;
      
      /* Return the address of the next object */
      rv = &next->next;

      /* Point free list to next object */
      bkeep->free_list = next->next;

      /* If this is the first allocation, decrement whole super blocks */
      if (bkeep->free_count == (SUPER_BLOCK_SIZE / bytes_per_object) - 1)
        pool->whole_superblocks -= 1;

      /* Decrement counts */
      pool->free_objects -= 1;
      bkeep->free_count -= 1;

      break;
    }

    /* Select the next superblock in the pool, returns NULL if none */
    bkeep = bkeep->next;
  }

  // assert that rv doesn't end up being NULL at this point
  assert(rv != NULL);

  /* Poison memory -- no need to account for next pointer */
  memset(rv, ALLOC_POISON, bytes_per_object );

  return rv;
}

static inline
struct superblock_bookkeeping * obj2bkeep (void *ptr) {
  uint64_t addr = (uint64_t) ptr;
  addr &= SUPER_BLOCK_MASK;
  return (struct superblock_bookkeeping *) addr;
}

void free(void *ptr) {
  struct superblock_bookkeeping *bkeep = obj2bkeep(ptr); 
  int bytes_per_object = (1 << (bkeep->level + 5));
  struct object* freed_obj = (struct object*) ptr;

  /* Poison memory: initialization of new object overwrites necessary bytes from poison */
  memset(freed_obj, FREE_POISON, bytes_per_object);

  /* Place the freed object on the front of the list */
  freed_obj->next = bkeep->free_list;
  bkeep->free_list = freed_obj;

  /* Increment counts */
  bkeep->free_count += 1;
  levels[bkeep->level].free_objects += 1;

  /* Check if superblock has no allocations */
  int max_free_objects = (SUPER_BLOCK_SIZE / bytes_per_object) - 1;
  if (max_free_objects == bkeep->free_count)
  	levels[bkeep->level].whole_superblocks += 1;

  int free_sb_objects = (SUPER_BLOCK_SIZE / bytes_per_object) - 1;
  int level = bkeep->level;

  /* Get first SB */
  bkeep = levels[level].next;
  /* Initialize prev SB to NULL */
  struct superblock_bookkeeping * prev = NULL;

  while (levels[level].whole_superblocks > RESERVE_SUPERBLOCK_THRESHOLD && bkeep != NULL) {

  	/* If the free count of the superblock is equal
  	   to the maximum allowed objects */ 
  	if (bkeep->free_count == free_sb_objects) {

  		/* Decrement counters */
  		levels[level].whole_superblocks -= 1;
  		levels[level].free_objects -= bkeep->free_count;

  		if (prev != NULL) {
  			/* If SB is NOT the front of the list */
  			prev->next = bkeep->next;
  		} else {
  			/* If SB is front of the list */
  			levels[level].next = bkeep->next;
  		}

  		munmap(bkeep, SUPER_BLOCK_SIZE);

  	} else {
	  	/* Get next SB */
	  	prev = bkeep;
	  	bkeep = bkeep->next;
	}
  }
}

// Do NOT touch this - this will catch any attempt to load this into a multi-threaded app
int pthread_create(void __attribute__((unused)) *x, ...) {
  exit(-ENOSYS);
}

