_static_examples: _lib2
	$(MAKE) -C ./examples/split_substrings
	$(MAKE) -C ./examples/hello_world
	$(MAKE) -C ./examples/integer_value
	$(MAKE) -C ./examples/printf_string
	$(MAKE) -C ./examples/length_of_string
	$(MAKE) -C ./examples/args
	$(MAKE) -C ./examples/print_contents_of_file
	$(MAKE) -C ./examples/brk_test
	$(MAKE) -C ./examples/smiley
	$(MAKE) -C ./examples/dog_trie
	$(MAKE) -C ./examples/malloc1
	

_shared_examples: _lib2
	# $(MAKE) -C ./examples/split_substrings shared 
	$(MAKE) -C ./examples/hello_world shared
	# $(MAKE) -C ./examples/integer_value shared
	# $(MAKE) -C ./examples/printf_string shared
	# $(MAKE) -C ./examples/length_of_string shared
	# $(MAKE) -C ./examples/args shared
	# $(MAKE) -C ./examples/print_contents_of_file shared
	# $(MAKE) -C ./examples/brk_test shared
	# $(MAKE) -C ./examples/smiley shared
	# $(MAKE) -C ./examples/dog_trie shared


_lib2:
	$(MAKE) -C ./lib2/
