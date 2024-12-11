#include "syscall.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "malloc.h"
#include "libc2.h"

#define true 1
#define false 0

#define ssize_t signed long long

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


extern int main(int argc, char* argv[]);




static void handle_format(char* string, size_t len, size_t offset){

}

int brk(void* addr) {
	return (int) sys_brk((size_t)addr);
}

/* increase program break by increment bytes */
void* sbrk(void* increment){
	unsigned long brk_current = brk(NULL);
	// if(brk_current == 0) {
	// 	brk_current = brk(NULL);
	// }

	brk((brk_current + increment));

	return (void*) brk_current;
}

int exit_code = 0;
void __deinit(){
	flush_stdout();
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
	// sys_write(1, "meow\n",5);
	char* args[argc];
	collect_args(argc, first_arg, (char**) &args);

	__init();
	exit_code = main(argc, (char**) &args);

	__deinit();
}




int tolower(int c) {
	unsigned char c2 = (unsigned char)c;
	
	if (c2 <'A' || c2 > 'Z') {
		return c2;
	} else {
		return c2 + 32;
	}

}

void perror(const char* str) {
	sys_write(2, str, strlen(str));
}
