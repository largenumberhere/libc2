#include "stdlib.h"
// #define PRIMITIVE_MALLOC 1


int abs(int j) {
	if (j < 0) {
		return -j;
	}

	return j;
}





extern int exit_code;
void exit(int status) {
	exit_code = status;
	__deinit();
}