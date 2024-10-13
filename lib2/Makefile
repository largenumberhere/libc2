_main:
	nasm -f elf64 sys.asm -o sys.a
	clang -Wnoincompatible-library-redeclaration -c -nostdlib libc2.c -o libc2.a -g
