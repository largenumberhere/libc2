 asm (".globl _start\n\t"
 	"_start:\n\t"
 	"ldr x0, [sp, #0]\n\t"
        "ldr x1, [sp, #8]\n\t"
	"b __c_entry\n\t");