_main:
	clang trie.c -Werror -Wextra -Wall -o trie -nostdlib -nostartfiles ../../lib2/libc2.a -g
	# gdb --args ./trie dog_names.txt 
	strace ./trie dog_names.txt