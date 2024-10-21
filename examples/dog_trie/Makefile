_main:
	clang trie.c -Werror -Wextra -Wall -o trie.elf -nostdlib -nostartfiles ../../lib2/libc2.a -g
	# gdb --args ./trie.elf dog_names.txt 
	strace ./trie.elf dog_names.txt