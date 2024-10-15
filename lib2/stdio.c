#include "stdio.h"
#include <stdarg.h>
#define ssize_t signed long long

static char file_buffers[128][32] = {0};
static size_t file_buffers_pos[32] = {0};
static size_t file_buffers_len[32] = {0};
static int eofs[32] = {0};

extern void write_i64(size_t val, char* out);

static void itoa(size_t val, int* len_out, char*buff_out) {
	// 32 is more than big enough because the max a size_t can hold is 20 digits long
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
	
	// reverse it
	int len = 0;
	for (ssize_t i = string_pos; i >=0; i--) {
		if (tmp[i] == '\0') {continue;}
		buff_out[len++] = tmp[i];
	}
	
	*len_out = len;

}

// static void itoa(size_t val, int* len_out, char* buff_out) {
// 	if(val == 0){
// 		*buff_out = '0';
// 		*len_out = 1;
// 		return;
// 	}

// 	// perform primitive conversion
// 	write_i64(val, buff_out);
// 	*len_out = 22;

// 	char* ptr = NULL;
	
// 	// stip away leading zeros
// 	int leading = 0;
// 	for(int i =1; i < *len_out; i++){
// 		if (buff_out[i] =='0') {
// 			leading+=1;
// 		}
// 		else{
// 			break;
// 		}
// 	}

// 	char tmp[32] = {0};
// 	if (buff_out[0]=='+') {
// 		for (int i = 0; i < 32; i++) {
// 			tmp[i] = buff_out[i+leading+1];
// 		}
		
// 	} else if (buff_out[0]=='-') {
// 		for (int i = 0; i < 32; i++) {
// 			tmp[i] = buff_out[i+leading];
// 			tmp[0] = buff_out[0];
// 		}
// 	}

// 	memcpy(buff_out, tmp, 22);

// 	int len = strlen(buff_out);
// 	*len_out = len;
// }

int fgetc(FILE* stream) {
	int fd = stream->fd;
	
	if (file_buffers_pos[fd] >= file_buffers_len[fd]) {
	
		if (eofs[fd]) {
			return (char) -1;
		}

		file_buffers_len[fd] = sys_read(fd, file_buffers[fd], 127);
		eofs[fd] = file_buffers_len[fd] < 127;
		file_buffers_pos[fd] = 0;
	}

	return file_buffers[fd][file_buffers_pos[fd]++];

	// int fd = stream->fd;
	// if (file_buffers_pos[fd] == 128) {
	// 	if (eof[fd] == true) {
	// 		return (char)-1;
	// 	}
	// 	int count = sys_read(fd, file_buffers[fd], 128);
	// 	if (count < 128) {
	// 		eof[fd] = true;
	// 	}
	// } 
	

	// return (char) file_buffers[fd][file_buffers_pos[fd]];

	
	// const char buffer[2] = {0};
	// size_t count = sys_read(stream->fd, buffer, 1);
	// //printf("%i", count);
	// if (count == 0) {
	// 	return (char)-1;
	// }

	// return (char) buffer[0];
}


int fclose(FILE* stream) {
	int fd = stream->fd;

	memset(file_buffers[fd],0,128);
	file_buffers_len[fd] = 0;
	file_buffers_pos[fd] = 0;

	sys_close(fd);

	return 0;	// success
}


static FILE_PTR* file_ptrs[32] = {0};
static FILE_PTR files[32] = {0};
static int files_len = 0;



#define mode_t int
FILE* fopen(const char* pathname, const char*mode) {
	#define O_CREAT 0100
	#define O_WRONLY 01

	if(files_len >=32) {
		return NULL;
	}

	file_ptrs[files_len] = & files[files_len];
	FILE_PTR* f = file_ptrs[files_len++];

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


static char putc_buffer[129] = {0}; 
static size_t putc_pos = 0;

void putc(char c, int stream) {
	if (stream == 1) {
		putc_buffer[putc_pos++] = c;
		
		if (putc_pos >= 128 || c=='\n') {
			sys_write(1, putc_buffer, putc_pos);
			putc_pos = 0;		
		} 
		
		
	}

	else {
		printf("unsupported stream used '%i'\n", stream);
	}
}

void flush_putc() {
	if (putc_pos > 0) {
	
		sys_write(1, putc_buffer, putc_pos);
		putc_pos = 0;		
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



static void write_int(int val) {
	int len;
	char buff[32] = {0};
	itoa((size_t)val, &len, &buff);
	
	puts_l(buff, len);
	//sys_write(1, buff, len);
	//sys_write(1, "write_int", 10);
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



int printf(char* format, ...){
	va_list ap;

	va_start(ap, format);
	//int a = va_arg(ap, int);
	

	char* fmt = format;

	size_t len = strlen(fmt);
	int arg_pos = 0;

	int ignore_count = 0;
	for(int i = 0; i < len; i++){
		if (ignore_count >0) {
			ignore_count--;
			continue;
		}

		int pc_pos = find_delim(fmt, '%', i);


		if (fmt[i] == '%') {
			if(i+1<len) {
		 		if (fmt[i+1]=='i' || fmt[i+1]=='d'){
					int a = va_arg(ap, int);
					write_int(a);
					ignore_count+=2;
				} else if (fmt[i+1] == 's') {
					char* s = va_arg(ap, char*);
					puts(s);
					//sys_write(1, (char*)s, strlen(s));				
					i+=1;
				} else if(fmt[i+1]=='c') {
					char c = va_arg(ap, char);
					putc(c, 1);
					i+=1;
					//ignore_count+=3;	
				} else {
					putc(fmt[i], 1);
				}

				
			}
		}
		else {
			// int buffer_pos = 0;
			// char buffer[65] = {0};
			// int i = 0;
			// for (; i < 64; i++) {
			// 	if(fmt[i] !='%' && fmt[i] != '\0') {
			// 		buffer[buffer_pos++] = fmt[i]; 
			// 	}
			// 	else {
			// 		break;
			// 	}
			// }

			// sys_write(1, buffer, i);

			// ignore_count+=i-1;

			// int left = 0;
			// for(; true; left++) {
			// 	if(fmt[left]!='%' || fmt[left]!='\0') {
			// 		break;
			// 	}
			// }
			// sys_write(1, fmt+i, left);
			// ignore_count+=left;
			putc(fmt[i], 1);
		}
	}
	

	va_end(ap);

	//sys_write(1, fmt, len);
	return 0;
}

