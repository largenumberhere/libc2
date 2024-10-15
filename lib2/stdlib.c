#include "stdlib.h"

int abs(int j) {
	if (j < 0) {
		return -j;
	}

	return j;
}

#define ssize_t signed long long
#define size_t unsigned long long

static ssize_t pointers = 0;
static void* alloc_start = NULL;
static size_t alloc_size = 0;
void* malloc(size_t size) {
	if (alloc_start == 0) {
		unsigned long brk_val = sbrk(0);
		alloc_start = (void*) brk_val;
	}

	void* allocated = (void*) sbrk(size);

	pointers +=1;
	alloc_size += size;

	return allocated;
} 

void *calloc(size_t nmemb, size_t size) {
	ssize_t res = (ssize_t) nmemb * (ssize_t) size;
	if (res < 0) {
		printf("Calloc failed because size was too big\n");
		return NULL;
	}

	// printf("allocating %i", (int)res);
	void* buff = malloc((size_t)res);
	// printf("clearing buffer\n");
	memset(buff, 0, res);

	// printf("allocated %i\n", (int)res);

	return buff;
}

void free(void* ptr) {
	if (ptr == NULL) {
		return;
	}

	pointers -=1;

	if (pointers < 0) {
		printf("Double free detected\n");
		return;
	}

	// de-allocate if all pointers are freed
	if (pointers <= 0) {
		brk((void*) alloc_start);
		alloc_size = 0;	
		return;
	}

	return;

}