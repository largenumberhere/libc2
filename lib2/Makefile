_main:
	nasm -f elf64 sys.asm -o sys.a
	clang -c ./syscall.c -o syscall.o
	clang -Wnoincompatible-library-redeclaration -c -nostdlib libc2.c -o libc2.o -Lsyscall.h -g
	ar cr libc2.a libc2.o syscall.o 