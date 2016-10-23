#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

int main() {
	void *x = malloc(1500);
	printf("Hello %p\n", x);
	// int i;
	// /* Testing size2level by allocating a number less than each level*/
	// for (i = 5; i < 12; i++) {
	// 	// x is allocated 2^i - 5 bytes
	// 	void *x = malloc((1 << i) - 5);
	// 	printf("Level %d:  %p\n", i - 5, x);
	// }
	// /* Further test by allocating exact sizes */
	// for (i = 5; i < 12; i++) {
	// 	// x is allocated 2^i - 5 bytes
	// 	void *x = malloc(1 << i);
	// 	printf("Level %d:  %p\n", i - 5, x);
	// }
	return (errno);
}


