#include "syscall.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "malloc.h"

// #include "libc2.a"



int printf(char* string, ...);
void perror(const char* str);
int tolower(int c);

void _start();
void __deinit();