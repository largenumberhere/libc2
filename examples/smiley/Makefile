colorize:
	rm -f smiley_out.bmp
	clang -ggdb3 -O0 -Qunused-arguments -std=c11 -Wall -Werror -Wextra -Wno-sign-compare -Wno-unused-parameter -Wno-unused-variable -Wshadow -o colorize.elf colorize.c helpers.c  -nostdlib  -nostartfiles ../../lib2/libc2.a -g	-L../../lib2/libc2.h
	# gdb --args ./colorize.elf smiley.bmp smiley_out.bmp
	strace ./colorize.elf smiley.bmp smiley_out.bmp
