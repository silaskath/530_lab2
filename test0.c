#include <stdio.h>
#include <stdlib.h>
int main() {
	int i, n, size;
	for(i = 4; i < 13; i++) {
		size = 1 << i;
		printf("Size of allocation: %d\n", size);
		void *x = malloc(size);
		printf("Location of allocation: %p\n", x);
		for(n = size; n < 4096; n += size) {
			void *z = malloc(size);
			printf("Location of allocation: %p\n", z);
		}
	}

	return 0;
}