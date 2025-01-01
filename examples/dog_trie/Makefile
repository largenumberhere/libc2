CC?=gcc

.PHONY: build
build:
	$(CC) trie.c -Werror -Wextra -Wall -o trie.elf -nostdlib -nostartfiles  ../../lib2/crt0.o ../../lib2/libc2.a -g
	
.PHONY: run
run:
	strace ./trie.elf dog_names.txt
