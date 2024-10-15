#ifndef STDIO_H
#define STDIO_H

#define size_t unsigned long long
#define NULL (void*)0
#define SEEK_CUR 1
typedef struct
{
	int fd;
} FILE_PTR;


#define FILE FILE_PTR

int fgetc(FILE* stream);
int fputc(int c, FILE* stream);
FILE* fopen(const char* pathname, const char*mode);
int fclose(FILE* stream);
size_t fwrite(const void * ptr, size_t size, size_t nmemb, FILE* stream) ;
int fseek(FILE* stream, long offset, int whence);
size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream);
void putc(char c, int stream);
void puts(const char* buffer);
int printf(char* format, ...);

void flush_putc();

#endif