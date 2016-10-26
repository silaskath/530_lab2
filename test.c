#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
int main() {
	/* (1) Check all possible memory allocation sizes */
	// int i, n, size;
	// for(i = 4; i < 13; i++) {
	// 	size = 1 << i;
	// 	printf("Size of allocation: %d\n", size);
	// 	void *x = malloc(size);
	// 	printf("Location of allocation: %p\n", x);
	// 	/* 4096 - size == is the size of the superblock 
	// 	   minus the bkeep object */
	// 	for(n = size; n < 4096/* - size */; n += size) {
	// 		void *z = malloc(size);
	// 		printf("Location of allocation: %p\n", z);
	// 	}
	// }

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
	void *test = malloc(1024);
	printf("Initial memory address: %p\n", test);
	free(test);
	void *replacement = malloc(1024);
	printf("After free, this should have same memory address: %p\n", replacement);
	
	void *t1 = malloc(1024);
	void *t2 = malloc(1024);
	void *t3 = malloc(1024);
	printf("1:%p\n2:%p\n3:%p\n", t1,t2,t3);
	free(t1);
	free(t2);
	free(t3);
	// the address of t4 == t3
	void *t4 = malloc(1024);
	printf("4:%p\n", t4);

	return 0;
}