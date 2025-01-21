# SOURCES=$(wildcard *.c)
# EXECUTABLE=$(SOURCES:.c=)

# $(EXECUTABLE): *.c

# $(EXECUTABLE): *.c
# all: ./*.c
# 	for file in $^ ; do \
# 		echo "Hello" $${file} ; \
# 	done	\

# 	$(CC) $^ -o $@-gcc.elf
# 	./$@-gcc.elf
# 	echo $@ passed

# 	$(CC) $^ -o $@-libc2.elf -nostdlib  -nostartfiles ../lib2/libc2.a -g  ../lib2/crt0.o	-L../lib2/libc2.h
# 	./$@-libc2.elf

CC?=gcc
LIBC2_FLAGS=-nostdlib  -nostartfiles ../lib2/libc2.a -g  ../lib2/crt0.o	-L../lib2/libc2.h
GLIBC_FLAGS=

.PHONY: all


all: clean strcmp1 strncmp1 strlen1 dprintf1

clean:
	rm -rf ./bin/*

.PHONY: strncmp1
strncmp1: strncmp1.c
	$(CC) $^ -o ./bin/$@-glibc.elf $(GLIBC_FLAGS)
	./bin/$@-glibc.elf

	$(CC) $^ -o ./bin/$@-libc2.elf $(LIBC2_FLAGS)
	./bin/$@-libc2.elf
	@echo Test: $@ passed


.PHONY: strcmp1
strcmp1: strcmp1.c
	$(CC) $^ -o ./bin/$@-glibc.elf $(GLIBC_FLAGS)
	./bin/$@-glibc.elf

	$(CC) $^ -o ./bin/$@-libc2.elf $(LIBC2_FLAGS)
	./bin/$@-libc2.elf
	echo Test: $@ passed

.PHONY: strlen1
strlen1: strlen1.c
	$(CC) $^ -o ./bin/$@-glibc.elf $(GLIBC_FLAGS)
	./bin/$@-glibc.elf

	$(CC) $^ -o ./bin/$@-libc2.elf $(LIBC2_FLAGS)
	./bin/$@-libc2.elf
	echo Test: $@ passed

.PHONY: dprintf1
dprintf1: dprintf1.c
	$(CC) $^ -o ./bin/$@-glibc.elf $(GLIBC_FLAGS)
	./bin/$@-glibc.elf

	$(CC) $^ -o ./bin/$@-libc2.elf $(LIBC2_FLAGS)
	./bin/$@-libc2.elf
	echo Test: $@ passed

