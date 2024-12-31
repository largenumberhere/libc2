.PHONY: build
build:
	clang -o main.elf ./main.c -nostdlib  -nostartfiles ../../lib2/libc2.a -g	-L../../lib2/libc2.h
	
.PHONY: run
run:
	./main.elf
