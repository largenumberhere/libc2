#ifndef LIBC2_H
#define LIBC2_H

#include "crt0.h"
#include "syscall.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "malloc.h"

// #include "libc2.a"



// int printf(char* string, ...);
void perror(const char* str);
int tolower(int c);

void _start();
void __deinit();

// unistd.h
int pipe(int file_descriptors_out[2]);
signed long long read(int fd, void *buf, unsigned long long count);

#endif