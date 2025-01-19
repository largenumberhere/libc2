#include "stdlib.h"
#include "libc2.h"
// #define PRIMITIVE_MALLOC 1


int abs(int j) {
	if (j < 0) {
		return -j;
	}

	return j;
}

static int exit_code;

#define _NO_RETURN_SUFFIX 
#define _NO_RETURN_PREFIX _Noreturn


_NO_RETURN_PREFIX void exit(int status) {
	exit_code = status;
	__deinit();
	while (1) {}
} _NO_RETURN_SUFFIX;