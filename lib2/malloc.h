#ifndef MALLOC_H
#define MALLOC_H
void* malloc(unsigned long long size);
void *calloc(unsigned long long nmemb, unsigned long long size);
void free(void* ptr);

#endif