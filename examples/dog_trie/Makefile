.PHONY: build
build:
	clang trie.c -Werror -Wextra -Wall -o trie.elf -nostdlib -nostartfiles ../../lib2/libc2.a -g
	
.PHONY: run
run:
	strace ./trie.elf dog_names.txt
