COPT = -nostdlib -g -O0 -L./*.h
NOPT = 

_main:
	rm -rf .ls/build/*

	clang -c ./malloc.c -o build/malloc.o $(COPT)
	clang -c ./stdio.c -o build/stdio.o $(COPT)
	clang -c ./stdlib.c -o build/stdlib.o $(COPT)
	clang -c ./string.c -o build/string.o $(COPT)
	clang -c ./syscall.c -o build/syscall.o $(COPT)
	
	clang -Wnoincompatible-library-redeclaration -c -nostdlib libc2.c -o build/libc2.o -L./*.h -g $(COPT)
	rm -f libc2.a
	ar cr libc2.a build/*.o

