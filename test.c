/*
 *  Authors: Winsten Marks, Silas Kath
 *
 *  UNC Honor Pledge: I, Silas Kath, certify that no unauthorized assistance has been received or
 *  given in the completion of this work.
 *
 *  UNC Honor Pledge: I, Winsten Marks, certify that no unauthorized assistance has been received or
 *  given in the completion of this work.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

/* Malloc tests */
int test_malloc() {
	printf("(1) Testing basic allocation\n");
	int i, size = 32;
	uint64_t prev = NULL;
	for (i = size; i < 4096; i += size) {
		char *ptr = malloc(size);
		if (prev != NULL) {
			/* The difference between each allocation should be exactly size (32) */
			assert(prev - (uint64_t) ptr == size);
		}
		prev = (uint64_t)ptr;
	}
	printf("Passed basic allocation...\n");

	printf("(2) Testing superblock allocation\n");
	size = 512;
	for (i = size; i < 4096 + size; i += size) {
		char *ptr = malloc(size);
		/* We know the next allocation creates a new superblock */
		if (size == 4096) {
			prev = ptr;
		}

		/* We also know that the difference between the old superblock
		   and the new should be 2 * size of an object, to account for bkeep */
		if (size > 4096) {
			assert(prev - (uint64_t) ptr == (2 * size));
		}
	}
	printf("Passed superblock allocation...\n");
	
	printf("(3) Testing malloced object can hold data\n");
	char *string = malloc(32);
	string = "Hello this is a string\n\0";
	printf("%s", string);
	printf("Test passed...\n");
	

	printf("(4) Test large number of allocations\n");
	for (i = 0; i < 1000; i++) {
		void *ptr = malloc(32);
	}
	printf("Passed large number of allocations...\n");

	printf("(5) Test allocation of each level\n");
	for (i = 5; i < 13; i++) {
		void *ptr = malloc(1 << i);
	}
	printf("Passed allocations of each level\n");


	return 1;	
}

void test_superblock_release() {
	printf("Testing the release of superblocks\n");
	printf("(1) Allocate three SBs, free them, and then alloc three more SBs"
		"\nbut with an interrupting malloc, two of the three mallocs should match the previous addresses\n");

	int size = 2048;

	void *sb1 = malloc(size);
	void *sb2 = malloc(size);
	void *sb3 = malloc(size);

	printf("sb1:%p\nsb2:%p\nsb3:%p\n", sb1, sb2, sb3);

	free(sb1);free(sb2);free(sb3);

	int i;
	void *sb4 = malloc(size);
	for (i = 0; i < 10; i++) {
		void *interrupt = malloc(512); 	// Allocate different page inbetween
	}
	void *sb5 = malloc(size);
	for (i = 0; i < 10; i++) {
		void *interrupt = malloc(512);	// So that allocations aren't linear
	}
	void *sb6 = malloc(size);

	printf("sb4:%p\nsb5:%p\nsb6:%p\n", sb4, sb5, sb6);
	printf("Test passed...\n");

	printf("(2) Simliar to last, but malloc a bunch of superblocks and free all of them in succession and remalloc\n");

	sb1 = malloc(size);sb2 = malloc(size);sb3 = malloc(size);
	sb4 = malloc(size);sb5 = malloc(size);sb6 = malloc(size);
	void *sb7 = malloc(size);

	printf("sb1:%p\nsb2:%p\nsb3:%p\nsb4:%p\nsb5:%p\nsb6:%p\nsb7:%p\n", sb1, sb2, sb3, sb4, sb5, sb6, sb7);

	free(sb1);free(sb2);free(sb3);free(sb4);free(sb5);free(sb6);free(sb7);

	sb1 = malloc(size);sb2 = malloc(size);sb3 = malloc(size);
	sb4 = malloc(size);sb5 = malloc(size);sb6 = malloc(size);sb7 = malloc(size);

	printf("sb1:%p\nsb2:%p\nsb3:%p\nsb4:%p\nsb5:%p\nsb6:%p\nsb7:%p\n", sb1, sb2, sb3, sb4, sb5, sb6, sb7);

	printf("Test passed...\n");
}

void test_free() {

	printf("(1) Test proper reallocation of freed objects (free then malloc)\n");

	int size = 512;

	void *t1 = malloc(size);void *t2 = malloc(size);
	void *t3 = malloc(size);void *t4 = malloc(size);

	uint64_t t1addr = (uint64_t) t1;uint64_t t2addr = (uint64_t) t2;
	uint64_t t3addr = (uint64_t) t3;uint64_t t4addr = (uint64_t) t4;

	printf("t1:%p\nt2:%p\nt3:%p\nt4:%p\n", t1, t2, t3, t4);

	free(t4);free(t3);
	free(t2);free(t1);

	void *t5 = malloc(size);void *t6 = malloc(size);
	void *t7 = malloc(size);void *t8 = malloc(size);

	uint64_t t5addr = (uint64_t) t5;uint64_t t6addr = (uint64_t) t6;
	uint64_t t7addr = (uint64_t) t7;uint64_t t8addr = (uint64_t) t8;

	printf("t5:%p\nt6:%p\nt7:%p\nt8:%p\n", t5, t6, t7, t8);

	/* free() is LIFO, meaning the last free, the memory address
	   of free(t1) should equal the memory address of the first allocation
	   malloc(t5) */
	assert(t1addr == t5addr);assert(t2addr == t6addr);
	assert(t3addr == t7addr);assert(t4addr == t8addr);

	printf("Proper reallocation test passed...\n");

	printf("(2) Test reallocation when freed objects span several superblocks (2048 bytes)\n");

	size = 2048;

	void *ptr1 = malloc(size);void *ptr2 = malloc(size);void *ptr3 = malloc(size);void *ptr4 = malloc(size);
	void *ptr5 = malloc(size);void *ptr6 = malloc(size);void *ptr7 = malloc(size);
	void *ptr8 = malloc(size);void *ptr9 = malloc(size);void *ptr10 = malloc(size);
	void *ptr11 = malloc(size);void *ptr12 = malloc(size);void *ptr13 = malloc(size);
	void *ptr14 = malloc(size);void *ptr15 = malloc(size);void *ptr16 = malloc(size);void *ptr17 = malloc(size);

	printf("\nptr1:%p\nptr2:%p\nptr3:%p\nptr4:%p\nptr5:%p\nptr6:%p\nptr7:%p\n", ptr1, ptr2, ptr3, ptr4, ptr5, ptr6, ptr7);
	printf("ptr8:%p\nptr9:%p\nptr10:%p\nptr11:%p\nptr12:%p\nptr13:%p\nptr14:%p\n", ptr8, ptr9, ptr10, ptr11, ptr12, ptr13, ptr14);
	printf("ptr15:%p\nptr16:%p\nptr17:%p\n", ptr15, ptr16, ptr17);

	free(ptr1);free(ptr2);free(ptr3);free(ptr4);
	free(ptr5);free(ptr6);free(ptr7);free(ptr8);
	free(ptr9);free(ptr10);free(ptr11);free(ptr12);
	free(ptr13);free(ptr14);free(ptr15);free(ptr16);free(ptr17);

	ptr1 = malloc(size);ptr2 = malloc(size);ptr3 = malloc(size);ptr4 = malloc(size);
	ptr5 = malloc(size);ptr6 = malloc(size);ptr7 = malloc(size);ptr8 = malloc(size);
	ptr9 = malloc(size);ptr10 = malloc(size);ptr11 = malloc(size);ptr12 = malloc(size);
	ptr13 = malloc(size);ptr14 = malloc(size);ptr15 = malloc(size);ptr16 = malloc(size);
	ptr17 = malloc(size);

	printf("\nptr1:%p\nptr2:%p\nptr3:%p\nptr4:%p\nptr5:%p\nptr6:%p\nptr7:%p\n", ptr1, ptr2, ptr3, ptr4, ptr5, ptr6, ptr7);
	printf("ptr8:%p\nptr9:%p\nptr10:%p\nptr11:%p\nptr12:%p\nptr13:%p\nptr14:%p\n", ptr8, ptr9, ptr10, ptr11, ptr12, ptr13, ptr14);
	printf("ptr15:%p\nptr16:%p\nptr17:%p\n", ptr15, ptr16, ptr17);

	printf("Test passed...\n");
	printf("(3) Testing excessive successive calls to malloc and free of "
		"\n2048 bytes (without freeing this would vastly exceed typical memory sizes)\n");
	int i;
	for (i = 0; i < 100000; i++) {
		int * ptr = malloc(2048);
		free(ptr);
	}
	printf("Passed 100,000 malloc and frees\n");
	printf("(4) Random mallocs and frees\n");
	void *r1 = malloc(32); void *r2 = malloc(22); void *r3 = malloc(50);
	void *r4 = malloc(532); void *r5 = malloc(1532); void *r6 = malloc(412);
	void *r7 = malloc(203); void *r8 = malloc(1); void *r9 = malloc(2047);
	free(r1);free(r8);free(r9);free(r3);
	r1 = malloc(2000); r8 = malloc(2000); r9 = malloc(2000);
	free(r1);
	r1 = malloc(1024);
	free(r8);free(r1);free(r2);free(r4);free(r5);free(r6);
	free(r7);free(r9);
	printf("Test passed...\n");
}

void test_poison() {
	printf("Test poison first.\n");
	int size = 32;
	char *c = malloc(size);
	void *c_loc = &c;
	printf("c_loc: %p, c: %p\n", c_loc, &c);
	int i;
	printf("Malloc Poison:\n");
	for(i = -10; i < size + 10; i++){
		if(i == -10)
			printf("Lines -10 through 0 should be unpoisoned data.\n");
		if(i == 0)
			printf("Lines 1 through 32 should be poisoned (cd).\n");
		if(i == 32)
			printf("Lines 33 through 42 should be unpoisoned.\n");
		printf("malloc %d: %x\n", i + 1, c[i] & 0xff);
	}
	printf("Free Poison:\n");
	free(c);
	for(i = -10; i < size + 10; i++){
		if(i == -10)
			printf("Lines -10 through 0 should be unpoisoned data.\n");
		if(i == 0)
			printf("Lines 1 through 8 should unpoisoned since they relate to the free list pointer.\n");
		if(i == 8)
			printf("Lines 9 through 32 should be poisoned (ab).\n");
		if(i == 32)
			printf("Lines 33 through 42 should be unpoisoned.\n");
		printf("free %d: %x\n", i + 1, c[i] & 0xff);
	}
}

// set environment LD_PRELOAD=./th_alloc.so
/* Calls series of tests */
void runTests() {
 	test_poison();
 	test_malloc();
 	test_free();
 	test_superblock_release();
}

int main() {
	runTests();
	return 1;
}
