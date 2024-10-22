_main:
	clang -o main.elf ./main.c -nostdlib  -nostartfiles ../../lib2/libc2.a -g	-L../../lib2/libc2.h
	./main.elf

shared:
	clang -o main.elf -nostartfiles ./main.c ../../lib2/libc2.so ../../lib2/crt0.o -L../../lib2/libc2.h
	LD_PRELOAD=../../lib2/libc2.so ./main.elf