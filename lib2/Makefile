COPT = -nostdlib -g -O0 -L./*.h -Wno-int-conversion -Wno-incompatible-library-redeclaration 
NOPT = 

_main:
	rm -rf ./build/*

	clang -c ./malloc.c -o build/malloc.o $(COPT)
	clang -c ./stdio.c -o build/stdio.o $(COPT)
	clang -c ./stdlib.c -o build/stdlib.o $(COPT)
	clang -c ./string.c -o build/string.o $(COPT)
	clang -c ./syscall.c -o build/syscall.o $(COPT)
	clang -c ./crt0.c -o build/crt0.o $(COPT)

	clang -c -nostdlib libc2.c -o build/libc2.o -L./*.h -g $(COPT)
	rm -f libc2.a
	ar cr libc2.a build/*.o

shared: _main
	rm -rf ./build/*
	clang -c ./malloc.c -o build/malloc.o $(COPT) -fPIC
	clang -c ./stdio.c -o build/stdio.o $(COPT) -fPIC
	clang -c ./stdlib.c -o build/stdlib.o $(COPT) -fPIC
	clang -c ./string.c -o build/string.o $(COPT) -fPIC
	clang -c ./syscall.c -o build/syscall.o $(COPT) -fPIC
	clang -c ./libc2.c -o build/libc2.o $(COPT) -fPIC
	clang -c ./crt0.c -o crt0.o $(COPT) -fPIC

	clang -shared -o libc2.so build/*.o -L./libc2.h
