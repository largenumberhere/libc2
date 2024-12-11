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

_Noreturn void exit(int status) {
	exit_code = status;
	__deinit();
	while (1) {}	// shuts up the compiler about returning
}