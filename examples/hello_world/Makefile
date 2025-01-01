CC?=gcc

.PHONY: build
build:
	$(CC) -o main.elf ./main.c -nostdlib  -nostartfiles ../../lib2/libc2.a -g ../../lib2/crt0.o	-L../../lib2/libc2.h

.PHONY: run
run:
	./main.elf


.PHONY: shared
shared:
	$(CC) -o main.elf -nostartfiles ./main.c ../../lib2/libc2.so ../../lib2/crt0.o -L../../lib2/libc2.h
	LD_PRELOAD=../../lib2/libc2.so ./main.elf



clean:
	rm -rf main.elf