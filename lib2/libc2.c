#include "syscall.h"

#include <stdarg.h>

#define true 1
#define false 0

#define size_t unsigned long long
#define ssize_t signed long long

#define NULL (void*)0

extern int main(int argc, char* argv[]);

static void puts_l(const char* buffer, size_t len);

static void handle_format(char* string, size_t len, size_t offset){

}

extern void write_i64(size_t val, char* out);

static void itoa(size_t val, int* len_out, char* buff_out) {
	if(val == 0){
		*buff_out = '0';
		*len_out = 1;
		return;
	}

	// perform primitive conversion
	write_i64(val, buff_out);
	*len_out = 22;

	char* ptr = NULL;
	
	// stip away leading zeros
	int leading = 0;
	for(int i =1; i < *len_out; i++){
		if (buff_out[i] =='0') {
			leading+=1;
		}
		else{
			break;
		}
	}

	char tmp[32] = {0};
	if (buff_out[0]=='+') {
		for (int i = 0; i < 32; i++) {
			tmp[i] = buff_out[i+leading+1];
		}
		
	} else if (buff_out[0]=='-') {
		for (int i = 0; i < 32; i++) {
			tmp[i] = buff_out[i+leading];
			tmp[0] = buff_out[0];
		}
	}

	memcpy(buff_out, tmp, 22);

	int len = strlen(buff_out);
	*len_out = len;
}

void memset(void* s, int c, size_t n){
	for(int i = 0; i < n; i++){
		*(char*)s = c;
	}
}

static void write_int(int val) {
	int len;
	char buff[32] = {0};
	itoa((size_t)val, &len, &buff);
	
	puts_l(buff, len);
	//sys_write(1, buff, len);
	sys_write(1, "write_int", 10);
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

// void putc(char c, int stream) {
// 	sys_write(1, &c, 1);
// }

static void flush_putc() {
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

void memcpy(void* dest, const void* src, size_t n) {
	for(size_t i = 0; i < n; i++){
		char* destc = (char*) dest;
		char* srcc = (char*) src;

		destc[i] =  srcc[i];
	}
}

static char* strtok_buff = NULL;
static int strtok_len = 0;
static int strtok_offset = 0;

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

char* strtok(char* str, char* delimiter) {
	int prev_offset = strtok_offset;

	if (str != NULL) {
		strtok_len = strlen(str);
		strtok_buff = str;
		strtok_offset = 0;
	} 

	if(strtok_offset > strtok_len) {
		return NULL;
	}
	
	int delim_pos = find_delim(strtok_buff, *delimiter, strtok_offset);
	strtok_offset = delim_pos+1;
	strtok_buff[strtok_offset-1] = '\0';

	return strtok_buff+prev_offset;	
}


/*
	untestsed

	return value:
		negative if str1 < str2
		pos if str1 > str1
		zero if str1 == str2
	"The sign of the result is the sign of the difference between the values of the
	 first pair of characters (both interpreted as unsigned char) that differ 
	 in the arrays being compared."
 */
int strncmp(const char* s1, const char* s2, size_t n) {
	size_t i = 0;
	
	for(; i<n; i++){
		if(s1[i]=='\0' || s2[i]=='\0') {
			break;
		}

		if(s1[i] != s2[i]) {
			break;
		}

	}

	int diff = ((unsigned char)s1[i] - (unsigned char)s2[i]);

	return diff;
}

int strcmp(const char* s1, const char* s2) {
	return strncmp(s1, s2, (size_t)-1);
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

static int exit_code = 0;
void __deinit(){
	flush_putc();
	sys_exit(exit_code);
}

void __init(){

}

static void collect_args(int argc, char* first_arg, char** out){
	int input_pos = 0;
	int output_pos = 0;
	for(int i = 0; i < argc; i++)
	{
		out[output_pos++] = first_arg+input_pos;
		//printf("read '%s'\n", out[output_pos-1]);
	
		int arg_len = strlen(first_arg+input_pos);
		input_pos+= arg_len+1;
	}
	
}

void __c_entry(int argc, char* first_arg) {
	char* args[argc];
	collect_args(argc, first_arg, &args);

	__init();
	exit_code = main(argc, &args);

	__deinit();
}



size_t strlen(const char* string){
	size_t i = 0;
	while(true){
		if (string[i] == '\0'){
			break;
		}
		i+=1;
	}

	return i;
}



// inline void sys_exit(int status){
// 	asm("movq %rax, $60");
// 	asm("syscall");
// }

//char* _start = asm("movq %rax, $60");

// inline void _start() {
// 	sys_exit(0);
// }

// extern void _start();

typedef struct
{
	int fd;
} FILE_PTR;

static FILE_PTR* file_ptrs[32] = {0};
static FILE_PTR files[32] = {0};
static int files_len = 0;

#define FILE FILE_PTR


#define mode_t int
FILE* fopen(const char* pathname, const char*mode) {
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
	else {
		return NULL;
	}

	return f;
}

// int open(const char* filename, int flags, mode_t mode) {

// }

int open(const char* filename, int flags) {
	return -1;
}




//extern size_t sys_read(int fd, char* buffer, size_t count);

static char file_buffers[128][32] = {0};
static size_t file_buffers_pos[32] = {0};
static size_t file_buffers_len[32] = {0};
static int eofs[32] = {0};

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

void exit(int status) {
	exit_code = status;
	__deinit();
}
