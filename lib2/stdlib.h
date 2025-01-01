
#ifndef STDLIB_H
#define STDLIB_H

#include "syscall.h"
#include "string.h"
#include "stdio.h"
#include "malloc.h"
#include "libc2.h"


#define NULL (void*)0

int abs(int j);

void exit(int status);


#endif