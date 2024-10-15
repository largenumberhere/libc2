_all_examples:	_examples_hello_world _examples_integer_value _length_of_a_string _split_substrings _printf_string _args _print_contents_of_file _brk_test _smiley

_split_substrings: _lib2
	$(MAKE) -C ./examples/split_substrings
	# ./examples/split_substrings/main

_examples_hello_world:	_lib2
	$(MAKE) -C ./examples/hello_world
	# ./examples/hello_world/main

_examples_integer_value:	_lib2
	$(MAKE) -C ./examples/integer_value
	# ./examples/integer_value/main

_printf_string: _lib2
	$(MAKE) -C ./examples/printf_string
	# ./examples/printf_string/main
_lib2:
	$(MAKE) -C ./lib2/

_length_of_a_string: _lib2
	$(MAKE) -C ./examples/length_of_string
	# ./examples/length_of_string/main

_args: _lib2
	$(MAKE) -C ./examples/args
	
_print_contents_of_file: _lib2
	$(MAKE) -C ./examples/print_contents_of_file

_brk_test: _lib2
	$(MAKE) -C ./examples/brk_test

_smiley: _lib2
	$(MAKE) -C ./examples/smiley
