_main:
	clang -o main.elf ./main.c -nostdlib  -nostartfiles ../../lib2/libc2.a -g	-L../../lib2/libc2.h
	strace ./main.elf
