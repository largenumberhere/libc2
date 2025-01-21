#ifndef STDIO_H
#define STDIO_H

#define size_t unsigned long long
#define NULL (void*)0
#define SEEK_CUR 1
typedef struct
{
	int fd;
	char buffer[128];
	size_t buffer_cur;
	size_t buffer_len;
	int eof;
} FILE_PTR;


#define FILE FILE_PTR

// streams 
static FILE *stdout;


// printf and scanf
int printf(char* format, ...);
int dprintf(int fd, char *format, ...);
int fprintf(FILE* stream, char* format, ...);
int fscanf(FILE *stream, const char *format, ...);
int __isoc99_fscanf(FILE* stream, const char* format, ...);


// files
int fgetc(FILE* stream);
int fputc(int c, FILE* stream);
FILE* fopen(const char* pathname, const char*mode);
int fclose(FILE* stream);
size_t fwrite(const void * ptr, size_t size, size_t nmemb, FILE* stream) ;
int fseek(FILE* stream, long offset, int whence);
size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream);
int __libc2_put_char(char c, int stream);
int putc(int c, FILE* stream);
int puts(const char* buffer);
int putchar(int c); 	

// unoffical
void flush_stdout();
void perror_int(int val);
#endif