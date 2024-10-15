#ifndef STRING_H
#define STRING_H

#define size_t unsigned long long
#define NULL (void*)0

void memset(void* s, int c, size_t n);
void memcpy(void* dest, const void* src, size_t n);
char* strtok(char* str, char* delimiter);
int strncmp(const char* s1, const char* s2, size_t n);
int strcmp(const char* s1, const char* s2);
size_t strlen(const char* string);

#endif