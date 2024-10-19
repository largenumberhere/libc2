#ifndef STDLIB_H
#define STDLIB_H
#define NULL (void*)0

int abs(int j);
void free(void* ptr);
void *calloc(unsigned long long nmemb, unsigned long long size);
void* malloc(unsigned long long size) ;

void exit(int status);


#endif