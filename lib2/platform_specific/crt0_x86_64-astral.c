#include "../libc2.h"

 asm (".globl _start\n\t"
 	"_start:\n\t"
 	"pop %rdi\n\t"
	"pop %rsi\n\t"
	"call __c_entry\n\t");