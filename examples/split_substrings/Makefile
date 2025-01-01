CC?=gcc

.PHONY: build
build:
	$(CC) -o main.elf ./main.c -nostdlib  -nostartfiles ../../lib2/libc2.a -g  ../../lib2/crt0.o	-L../../lib2/libc2.h

.PHONY: run
run:
	./main.elf
