#include "stdio.h"
#include "syscall.h"
#include "stdlib.h"
#include "libc2.h"
#include "string.h"
#include <stdarg.h>
#include "malloc.h"
#include "debug_util.h"

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
		if (tmp[i] == '\0') {continue;}
		buff_out[len++] = tmp[i];
	}
	
	*len_out = len;

}

static void itoa_hex(size_t val, int* len_out, char*buff_out) {
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
		c = val % 16;
		val = val / 16;
		m = (char) '0' + c;
		
		// adjust character for a to f
		if (m > '9') {
			m+=39;
		}

		tmp[string_pos++] = m;
	}
	
	size_t i = string_pos;
	while(tmp[i]=='\0') {
		i--;
	}

	// reverse it
	int len = 0;
	for (ssize_t i = (ssize_t)string_pos; i >=0; i--) {
		//if (tmp[i] == '\0') {continue;}
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
		return -1;
	}

}



static char stdout_buffer[129] = {0}; 
static size_t stdout_buffcur = 0;

void putc(char c, int stream) {
	if (stream == 1) {
		stdout_buffer[stdout_buffcur++] = c;
		
		if (stdout_buffcur >= 128 || c=='\n') {
			sys_write(1, stdout_buffer, stdout_buffcur);
			stdout_buffcur = 0;		
		} 
		
		
	}
	else {
		printf("unsupported stream used '%i'\n", stream);
		UNIMPLEMENTED();
	}
}

void flush_stdout() {
	if (stdout_buffcur > 0) {
		sys_write(1, stdout_buffer, stdout_buffcur);
		stdout_buffcur = 0;		
	}
}

void puts(const char* buffer) {
	size_t len = strlen(buffer);
	for (size_t i = 0; i < len; i++) {
		putc(buffer[i], 1);
	}
}

static void puts_l(const char* buffer, size_t len) {
	for(size_t i = 0; i < len; i++) {
		putc(buffer[i], 1);
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
		putc('-', 1);
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

int printf(char* format, ...){
	va_list pva;

	va_start(pva, format);
	
	size_t len = strlen(format);
	int arg_pos = 0;

	// int ignore_count = 0;
	int i = 0;
	for(; i < len; ){
		if (strncmp(format+i, "%i", 2)==0 || strncmp(format+i, "%d", 2) == 0) {
			int a = va_arg(pva, int);
			write_int(a);
			i+=2;
		} else if (strncmp(format+i, "%s", 2) == 0) {
			char* s = va_arg(pva, char*);
			puts(s);
			i+=2;
		} else if (strncmp(format+i, "%c", 2) == 0) {
			char c = (char) va_arg(pva, int);
			putc(c, 1);
			i+=2;
		} else if (strncmp(format+i, "%%",2) == 0) {
			putc('%', 1);
			i+=2;
		}
		else if(strncmp(format+i, "%x",2)==0) {
			size_t h = va_arg(pva, size_t);
			write_hex((size_t)h);
			i+=2;
		} else if (strncmp(format+i, "%",1)==0) {
			perror(format);
			UNIMPLEMENTED();
			
		} else {
			putc(format[i], 1);
			i+=1;
		}
	}
	
	va_end(pva);

	return 0;
}

