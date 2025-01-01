CC?=gcc

.PHONY: build
build:
	$(CC) -o main.elf ./main.c -nostdlib  -nostartfiles ../../lib2/libc2.a -g	-L../../lib2/libc2.h

.PHONY: run
run:
	strace ./main.elf
