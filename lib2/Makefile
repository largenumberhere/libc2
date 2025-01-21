CFLAGS+=-nostdlib -g -O0 -ffreestanding

CC?=gcc
# assume the user is compiling for this platform by default
TARGET?=$(shell gcc -dumpmachine)

_main:
	rm -rf ./build/*
	
	$(CC) -c ./malloc.c -o build/malloc.o $(CFLAGS)
	$(CC) -c ./stdio.c -o build/stdio.o $(CFLAGS)
	$(CC) -c ./stdlib.c -o build/stdlib.o $(CFLAGS)
	$(CC) -c ./string.c -o build/string.o $(CFLAGS)
	$(CC) -c ./platform_specific/syscall_$(TARGET).c -o build/syscall.o $(CFLAGS)
	$(CC) -c ./platform_specific/crt0_$(TARGET).c -o crt0.o $(CFLAGS)
	$(CC) -c ./rand.c -o build/rand.o $(CFLAGS)

	$(CC) -c -nostdlib libc2.c -o build/libc2.o -g $(CFLAGS)
	rm -f libc2.a
	ar cr libc2.a build/*.o
	
	# cp build/crt0.o ./crt0.o

shared: _main
	rm -rf ./build/*
	$(CC) -c ./malloc.c -o build/malloc.o $(CFLAGS) -fPIC
	$(CC) -c ./stdio.c -o build/stdio.o $(CFLAGS) -fPIC
	$(CC) -c ./stdlib.c -o build/stdlib.o $(CFLAGS) -fPIC
	$(CC) -c ./string.c -o build/string.o $(CFLAGS) -fPIC
	$(CC) -c ./platform_specific/syscall_$(TARGET).c -o build/syscall.o $(CFLAGS) -fPIC
	$(CC) -c ./libc2.c -o build/libc2.o $(CFLAGS) -fPIC
	$(CC) -c ./platform_specific/crt0_$(TARGET).c -o crt0.o $(CFLAGS) -fPIC

	$(CC) -shared -o libc2.so build/*.o

clean:
	rm -rf build/*.o
	rm -f ./libc2.a
	rm -f ./crt0.o
	rm -f ./libc2.so
