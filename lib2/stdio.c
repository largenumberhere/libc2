#include "stdio.h"
#include "syscall.h"
#include "stdlib.h"
#include "libc2.h"
#include "string.h"
#include <stdarg.h>
#include "malloc.h"
#include "debug_util.h"
#include <stdint.h>
#include <stdbool.h>

#define ssize_t signed long long


// static char file_buffers[128][32] = {0};
// static size_t file_buffers_pos[32] = {0};
// static size_t file_buffers_len[32] = {0};
// static int eofs[32] = {0};


// extern void write_i64(size_t val, char* out);

static void itoa(size_t val, int* len_out, char*buff_out) {
	// 32 is more than big enough because the max a size_t can hold is 20 digits long
	

	if (val ==0) {
		buff_out[0] = '0';
		*len_out = 1;
		return;
	}
	
	char tmp[32] = {0};

	size_t string_pos = 0;

	size_t c = 0;
	char m = '\0';

	while (val > 0)
	{
		c = val % 10;
		val = val / 10;
		m = (char) '0' + c;
		tmp[string_pos++] = m;
	}


	int len = 0;
	for (ssize_t i = string_pos; i >=0; i--) {
		if (tmp[i] == '\0') {continue;}	// workaround for the off-by-one bug hidden in here
		buff_out[len++] = tmp[i];
	}
	
	*len_out = len;

}

static void itoa_hex(size_t val, int* len_out, char*buff_out) {
	// 32 is more than big enough because the max a size_t can hold is `0x7FFFFFFFFFFFFFFF`, 16 digits long
	
	if (val ==0) {
		buff_out[0] = '0';
		*len_out = 1;
		return;
	}
	
	char tmp[32] = {0};

	size_t string_pos = 0;

	size_t c = 0;
	char m = '\0';

	while (val > 0)
	{
		c = val % 16;
		val = val / 16;
		m = (char) '0' + c;
		
		// adjust character for a to f
		if (m > '9') {
			m+=39;
		}

		tmp[string_pos++] = m;
	}
	
	// reverse it
	int len = 0;
	for (ssize_t i = (ssize_t)string_pos; i >=0; i--) {
		if (tmp[i] == '\0') {continue;}	// workaround for the off-by-one bug hidden in here
		buff_out[len++] = tmp[i];
	}
	
	*len_out = len;

}

#define assert(condition)	\
	do {\
		int __assert_val = condition;\
		if (__assert_val != 1) {	\
			printf("Assert failed at %s:%i. Result was %i\n", __FILE__, __LINE__, __assert_val);\
			exit(1);\
		}\
	} while(0);

// read file functions
size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream) {
	size_t i = 0;
	
	for (; i < size*nmemb; i++) {
		char f = fgetc(stream);
		if ((char)i == -1) {
			break;
		}
		((char*)ptr)[i] = f;
	}
	
	return i;
}

int fgetc(FILE* stream) {
	int fd = stream->fd;


	// fill up the buffer if at the end of it
	if (stream->buffer_cur >= stream->buffer_len) {
		if (stream->eof) {
			return (char) -1;
		}

		stream->buffer_len = sys_read(fd, stream->buffer, 127);
		stream->eof = stream->buffer_len < 127;
		stream->buffer_cur = 0;
	}

	// return next character
	return stream->buffer[stream->buffer_cur++];

}

void file_deallocate(FILE* f) {
	memset(f, 0, sizeof(FILE));
	free((void*)f);
}

int fclose(FILE* stream) {
	int fd = stream->fd;

	sys_close(fd);
	file_deallocate(stream);


	return 0;	// success
}

#define mode_t int

FILE* file_allocate() {
	FILE* f;
	f = (FILE*) calloc(1, sizeof(FILE));
	assert(f!=NULL);

	return f;
}


void file_destroy(FILE* f) {
	memset(f, 0, sizeof(FILE));
	f->fd = -2;
}


FILE* fopen(const char* pathname, const char*mode) {
	#define O_CREAT 0100
	#define O_WRONLY 01

	// if(files_len >=32) {
	// 	return NULL;
	// }

	// file_ptrs[files_len] = & files[files_len];
	// FILE_PTR* f = file_ptrs[files_len++];
	// return f;

	FILE_PTR* f = file_allocate();

	// file_ptrs[files_len] = & files[files_len];
	// FILE_PTR* f = file_ptrs[files_len++];

	if (mode[0] == 'r' && mode[1] =='\0') {
		mode_t modet = 0700;	// octal number
		int fd = sys_open(pathname, 0, modet);
		f->fd = fd;
	}
	else if(mode[0] == 'w' && mode[1] == '\0') {
		mode_t modet = 0070;
		int fd = sys_open(pathname,O_CREAT | O_WRONLY, modet);
		f->fd = fd;
	}
	else {
		return NULL;
	}

	return f;
}

// write file functions
// return the character written as an unsigned char cast to an int or EOF on error.
int fputc(int c, FILE* stream) {
	sys_write(stream->fd, &c, 1);
	return (unsigned char) c;
}

size_t fwrite(const void * ptr, size_t size, size_t nmemb, FILE* stream) {
	return sys_write(stream->fd, (char*)ptr, nmemb*size);
}


int fseek(FILE* stream, long offset, int whence) {
	if (whence == SEEK_CUR) {
		for(size_t i = 0; i < offset; i++) {
			int _ =fgetc(stream);
		}

		return 0;
	} else {
		UNIMPLEMENTED();
		return -1;
	}

}



static char stdout_buffer[129] = {0}; 
static size_t stdout_buffcur = 0;


int putc(int c, FILE* stream) {
	return fputc(c, stream);
}

// return the character written as an unsigned char cast to an int or EOF on error.
int __libc2_put_char(char c, int stream) {
	sys_write(stream, &c, 1);
	return (int) (unsigned char) c;
	// if (stream == 1) {
	// 	stdout_buffer[stdout_buffcur++] = c;
		
	// 	if (stdout_buffcur >= 128 || c=='\n') {
	// 		sys_write(1, stdout_buffer, stdout_buffcur+1);
	// 		stdout_buffcur = 0;		
	// 	} 
		
		
	// }
	// else {
	// 	printf("unsupported stream used '%i'\n", stream);
	// 	UNIMPLEMENTED();
	// }
}


void flush_stdout() {
	if (stdout_buffcur > 0) {
		sys_write(1, stdout_buffer, stdout_buffcur);
		stdout_buffcur = 0;		
	}
}

// return the character written as an unsigned char cast to an int or EOF on error.
int putchar(int c) {
	__libc2_put_char(c, 1);
	return (unsigned char) c;
}	

// return a nonnegative number on success, or EOF on error.
int puts(const char* buffer) {
	size_t len = strlen(buffer);
	for (size_t i = 0; i < len; i++) {
		__libc2_put_char(buffer[i], 1);
	}

	return len;
}

static void puts_l(const char* buffer, size_t len) {
	for(size_t i = 0; i < len; i++) {
		__libc2_put_char(buffer[i], 1);
	}
}


static void write_hex(size_t val) {
	int len;
	char buff[32] = {0};
	itoa_hex(val, &len, buff);
	puts_l("0x",2);
	puts_l(buff, len);
}

static void write_int(int val) {
	if (val < 0) {
		__libc2_put_char('-', 1);
		val = -val;
	}

	int len;
	char buff[32] = {0};
	itoa((size_t)val, &len, (char*) &buff);
	
	puts_l(buff, len);
	//sys_write(1, buff, len);
	//sys_write(1, "write_int", 10);
}

void perror_int(int val) {
	int len;
	char buff[32] = {0};
	itoa((size_t)val, &len, (char*) &buff);
	perror(buff);
}

static int find_delim(char* str, char delim, int offset) {
	int i = offset;

	char* at = str+offset;
	while(*at!='\0') {
		if (*at == delim) {
			return i;
		}

		at++;
		i++;
	}

	return i;
}

static int _fscanf_impl(FILE* stream, const char* format, va_list ap) {
	int matches = 0;
	size_t len = strlen(format);

	for (size_t i = 0; i < len; i++) {
		if(strncmp(format+i, "%s", 2)==0) {
			char* out_str = va_arg(ap, char*);
			size_t out_cur = 0;
			int c = fgetc(stream);
			while(c!='\n' && c!=' ' && c!=-1) {
				out_str[out_cur++] = (char)c;
				c = fgetc(stream);
			}
			out_str[out_cur++] = '\0';
			if (out_cur>1) {
				matches+=1;	
			}
		} else if(strncmp(format+1, "%", 1)==0) {
			UNIMPLEMENTED();
		}
	}



	// printf("scanned\n");

	return matches;

}

int fscanf(FILE* stream, const char* format, ...) {
	va_list ap;
	va_start(ap, format);
	int val = _fscanf_impl(stream, format, ap);
	va_end(ap);

	return val;
}



//int fscanf(FILE *stream, const char *format, ...) {
int __isoc99_fscanf(FILE* stream, const char* format, ...) {
	va_list ap;
	va_start(ap, format);
	int val = _fscanf_impl(stream, format, ap);
	va_end(ap);

	return val;
}

// check for the existence of b in the start of a
static bool has_prefix(char* string, char* b) {
        size_t b_len = strlen(b);

        int value = strncmp(string, b, b_len);

        if (value == 0) {
                return true;
        }
        else {
                return false;
        }

}



static int __fmt(int fd, char* format, va_list *args) {
	if (fd!=1) {
		perror("only file descriptor one is currently supported");
		UNIMPLEMENTED()				
	} 

	size_t format_len = strlen(format);
	
	// check for plain format string
	bool has_percent = false;
	for (ssize_t i = 0; i < format_len; i++) {
		if (format[i] == '%') {
			has_percent = true;
			break;
		}
	}
	
	// handle plain string
	if (!has_percent) {
		for (ssize_t i = 0; i < format_len; i++) {
			__libc2_put_char(format[i], fd);
		}

		return format_len;
	}
	
	// handle formatted string
	int printed_count = 0;
	ssize_t cursor = 0;
	while (true) {
		if (cursor >= format_len) {
			break;
		}

		// likely a format specifier
		if (format[cursor] == '%') {
			cursor++;
			// bounds check
			if (cursor >= format_len) {
				// treat a trailing percent like a normal character
				__libc2_put_char(format[cursor], fd);
				cursor++;
				printed_count++;
				continue;
			}

			// check for another percent sign 
			if (has_prefix(format+cursor, "%")) {
				// no format, just a percent sign
				__libc2_put_char(format[cursor], fd);
				printed_count++;
				cursor++;
				continue;
			
			} else if (has_prefix(format+cursor, "i") || has_prefix(format+cursor, "d")) {
				// integer format
				int value = va_arg(*args, int);
				write_int(value);
				printed_count ++; // todo: make this correct
				cursor++;
				continue;

			} else if (has_prefix(format+cursor, "x")) {
				// hex integer format
				int value = va_arg(*args, int);
				write_hex((size_t)value);
				printed_count ++;	// todo: make this correct
				cursor++;
				continue;

			} else if (has_prefix(format+cursor, "llx")) {
				// long long integer format
				size_t value = va_arg(*args, size_t);
				write_hex(value);
				printed_count ++;	// todo: make this correct
				cursor+=3;
				continue;

			} else if (has_prefix(format+cursor, "s")) {
				// string format
				char* string = va_arg(*args, char*);
				for (ssize_t i = 0; i < strlen(string) ;i++) {
					__libc2_put_char(string[i], fd);
					printed_count ++;
				}
				cursor++;
				continue;

			} else if (has_prefix(format+cursor, "c")) {
				// char format
				char c = (char) va_arg(*args, int);	// va_arg promotes small sizes up to ints of to 32-bits
				__libc2_put_char(c, fd);
				cursor+=1;
				continue;

			} 
			
			else {
				UNIMPLEMENTED()
			}
		} else {
			__libc2_put_char(format[cursor], fd);
			cursor ++;
		}
	}

	return printed_count;
	
}

int dprintf(int fd, char *format, ...) {
	va_list args;
	va_start(args, format);
	

	int count = __fmt(fd, format, &args);


	va_end(args);

	return count;
}


int printf(char *format, ...) {
	va_list args;
	va_start(args, format);

	
	int count = __fmt(1, format, &args);

	va_end(args);

	return count;

}

// int printf(char* format, ...){
// 	va_list pva;

// 	va_start(pva, format);
	
// 	size_t len = strlen(format);
// 	// int arg_pos = 0;

// 	if (len == 0) {
// 		return 0;
// 	}

// 	// int ignore_count = 0;
// 	int i = 0;
// 	for(; i < len; ){
// 		if (strncmp(format+i, "%i", 2)==0 || strncmp(format+i, "%d", 2) == 0) {
// 			int a = va_arg(pva, int);
// 			write_int(a);
// 			i+=2;
// 		} else if (strncmp(format+i, "%s", 2) == 0) {
// 			char* s = va_arg(pva, char*);
// 			puts(s);
// 			i+=2;
// 		} else if (strncmp(format+i, "%c", 2) == 0) {
// 			char c = (char) va_arg(pva, int);
// 			__libc2_put_char(c, 1);
// 			i+=2;
// 		} else if (strncmp(format+i, "%%",2) == 0) {
// 			__libc2_put_char('%', 1);
// 			i+=2;
// 		}
// 		else if(strncmp(format+i, "%x",2)==0) {
// 			int h = va_arg(pva, int);
// 			write_hex((size_t)h);
// 			i+=2;
// 		} else if (strncmp(format+i, "%llx", 4) == 0) {
// 			size_t h = va_arg(pva, size_t);
// 			write_hex((size_t)h);
// 			i+=4;

// 		} else if (strncmp(format+i, "%",1)==0) {
// 			perror(format);
// 			UNIMPLEMENTED();
// 			i+=1;
// 		} else {
// 			__libc2_put_char(*(format+i), 1);
// 			i+=1;
// 		}
// 	}
	
// 	va_end(pva);

// 	return 0;
// }

