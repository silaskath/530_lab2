#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* Check all possible memory allocation sizes */
int mallocT1() {
	int i, n, size;
	for(i = 4; i < 13; i++) {
		size = 1 << i;
		printf("Size of allocation: %d\n", size);
		void *x = malloc(size);
		printf("Location of allocation: %p\n", x);
		/* 4096 - size == is the size of the superblock 
		   minus the bkeep object */
		for(n = size; n < 4096/* - size */; n += size) {
			void *z = malloc(size);
			printf("Location of allocation: %p\n", z);
		}
	}
	return 1;	
}

/* Let's run out of memory */
int mallocT2() {
	int i;
	for (i = 0; i < 1000; i++) {
		void *ptr = malloc(512);
		printf("%d: %p\n", i, ptr);
		// if (i % )
		// 	free(ptr);
	}
	return 1;
}

void test_superblock_release() {
	void *sb1 = malloc(2048);
	void *sb2 = malloc(2048);
	void *sb3 = malloc(2048);

	printf("sb1:%p\nsb2:%p\nsb3:%p\n", sb1, sb2, sb3);

	free(sb1);
	free(sb2);
	free(sb3);

	sb1 = malloc(2048);
	sb2 = malloc(2048);
	sb3 = malloc(2048);

	printf("sb1:%p\nsb2:%p\nsb3:%p\n", sb1, sb2, sb3);
}

int freeTest1() {
	int size = 256;
	void *t1 = malloc(size);
	void *t2 = malloc(size);
	void *t3 = malloc(size);
	printf("1:%p\n2:%p\n3:%p\n", t1,t2,t3);
	free(t3);
	free(t2);
	free(t1);
	void *t4 = malloc(size);
	printf("4:%p\n", t4);
	void *t5 = malloc(size);
	printf("5:%p\n", t5);
	void *t6 = malloc(size);
	printf("6:%p\n", t6);

	// free(t3);
	// t3 = malloc(size);
	// printf("7:%p\n", t3);
	// printf("t3:%p\n", t3);
	return 1;
}
// set environment LD_PRELOAD=./th_alloc.so
/* Calls series of tests */
int runTests() {
 	// mallocT1();
 	// freeTest1();
 	test_superblock_release();
 	// mallocT2();
	return 1;
}

int main() {

	return runTests();
	// // void *shit = malloc(32);
	// // printf("address of shit(32bytes): %p\n", shit);
	// // void *more = malloc(64);
	// // printf("address of more(64bytes): %p\n", more);
	// // void *moreshit = malloc(32);
	// // printf("address of shit(32bytes): %p\n", moreshit);

	// void *shit = malloc(32);
	// printf("address of shit(32bytes): %p\n", shit);
	// void *more = malloc(64);
	// printf("address of more(64bytes): %p\n", more);
	// void *moreshit = malloc(32);
	// printf("address of shit(32bytes): %p\n", moreshit);

	// printf("shit - moreshit: %ld\n", (uint64_t)shit - (uint64_t)moreshit);
	// char *ptr = "Hello my name is Bob\n\0";
	// printf("%s\n", ptr);





	/* FREEDOM */
	// void *test = malloc(1024);
	// printf("Initial memory address: %p\n", test);
	// free(test);
	// void *replacement = malloc(1024);
	// printf("After free, this should have same memory address: %p\n", replacement);
	
	



}
