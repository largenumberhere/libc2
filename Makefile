.PHONY: lib2
.PHONY: _static_examples
.PHONY: _shared_examples
.PHONY: examples
.PHONY: clean

CC?=gcc

lib2:
	$(MAKE) -C ./lib2/
	$(MAKE) shared -C ./lib2
	echo "Finished compiling library. Try out examples with 'make examples'"


tests:
	$(MAKE) -C ./unit_tests/


# todo: fixed shared (seperate them into run and build steps) 
examples:  _static_examples
	echo "Finished building examples. Try out 'make run_examples'"


run_examples: 
	# $(MAKE) run -C ./examples/*
	$(MAKE) run -C ./examples/hello_world
	$(MAKE) run -C ./examples/integer_value
	$(MAKE) run -C ./examples/printf_string
	$(MAKE) run -C ./examples/length_of_string
	$(MAKE) run -C ./examples/args
	$(MAKE) run -C ./examples/print_contents_of_file
	$(MAKE) run -C ./examples/brk_test
	$(MAKE) run -C ./examples/smiley
	$(MAKE) run -C ./examples/dog_trie
	$(MAKE) run -C ./examples/malloc1


_static_examples:
	$(MAKE) build -C ./examples/split_substrings
	$(MAKE) build -C ./examples/hello_world
	$(MAKE) build -C ./examples/integer_value
	$(MAKE) build -C ./examples/printf_string
	$(MAKE) build -C ./examples/length_of_string
	$(MAKE) build -C ./examples/args
	$(MAKE) build -C ./examples/print_contents_of_file
	$(MAKE) build -C ./examples/brk_test
	$(MAKE) build -C ./examples/smiley
	$(MAKE) build -C ./examples/dog_trie
	$(MAKE) build -C ./examples/malloc1
	

_shared_examples:
	# $(MAKE) build -C ./examples/split_substrings shared 
	$(MAKE) build -C ./examples/hello_world shared
	# $(MAKE) build -C ./examples/integer_value shared
	# $(MAKE) build -C ./examples/printf_string shared
	# $(MAKE) build -C ./examples/length_of_string shared
	# $(MAKE) build -C ./examples/args shared
	# $(MAKE) build -C ./examples/print_contents_of_file shared
	# $(MAKE) build -C ./examples/brk_test shared
	# $(MAKE) build -C ./examples/smiley shared
	# $(MAKE) build -C ./examples/dog_trie shared


clean:
	$(MAKE) clean -C ./lib2

	$(MAKE) clean -C ./examples/hello_world
	$(MAKE) clean -C ./examples/integer_value
	$(MAKE) clean -C ./examples/printf_string
	$(MAKE) clean -C ./examples/length_of_string
	$(MAKE) clean -C ./examples/args
	$(MAKE) clean -C ./examples/print_contents_of_file
	$(MAKE) clean -C ./examples/brk_test
	$(MAKE) clean -C ./examples/smiley
	$(MAKE) clean -C ./examples/dog_trie
	$(MAKE) clean -C ./examples/malloc1
