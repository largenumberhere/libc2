 asm (".globl _start\n\t"
 	"_start:\n\t"
 	"ldp x0, x1, [SP, -16]  \n\t"
	"b __c_entry\n\t");