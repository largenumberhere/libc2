# Libc2
A naive implementation of libc for linux x86_64.
This implementation is purposed to be simple and dependency-free for educational value. It implements a subset of libc without full feature support or exhaustive error handling. 
Feedback is encouragd.

# How to try it
1. use `make` to build the library*
2. use `make examples` to build the static examples
3. use `make run_examples` to run the static examples

# Non x86 targets
Disclaimer: This library is intended primarily for x86_64 so support for other platforms will be limited.
If you are not using the target x86_64-gnu-linux, you will need pass in the '
platform triple'. You can get your target triple with the command `gcc -dumpmachine`.
- For example, arm64 uses `TARGET=aarch64-linux-gnu make` will build the library.
- `TARGET=x86_64-astral make`