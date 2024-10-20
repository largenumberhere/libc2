#ifndef DEBUG_UTIL_H
#define DEBUG_UTIL_H
#include "stdio.h"
#define UNIMPLEMENTED() \
	perror("Unimplemented! ");\
	perror(__FILE__);\
	perror(":");\
	perror_int(__LINE__);\
	perror("\n");\
	exit(1);


#endif