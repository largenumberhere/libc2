CC?=gcc

.PHONY: build
build:
	rm -f smiley_out.bmp
	$(CC) -ggdb3 -O0 -std=c11 -Wall -Wextra -Wno-sign-compare -Wno-unused-parameter -Wno-unused-variable -Wshadow -o colorize.elf colorize.c helpers.c  -nostdlib  -nostartfiles ../../lib2/libc2.a -g  ../../lib2/crt0.o	-L../../lib2/libc2.h

.PHONY: run
run:
	# gdb --args ./colorize.elf smiley.bmp smiley_out.bmp
	./colorize.elf smiley.bmp smiley_out.bmp

clean:
	rm -rf main.elf