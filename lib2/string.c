#include "string.h"
#include <stdint.h>
#include "debug_util.h"
#define size_t unsigned long long

void* memset(void* s, int c, size_t n){
	char* string = s;
	
	char eight[8];
	if (n > sizeof(uint32_t) || n > sizeof(uint64_t)) {
		// initalize eight only if it may be read from
		for (int i = 0; i < sizeof(uint64_t); i++) {
			eight[i] = (unsigned char) c;
		} 
	}
	while (n > sizeof(uint64_t)) {
		// copy in 8-byte chunks
		*(uint64_t*)string = *(uint64_t*) eight;
		s += sizeof(uint64_t);
		n -= sizeof(uint64_t);
	}
	while (n > sizeof(uint32_t)) {
		// copy in 4-byte chunks
		*(uint32_t*)string = *(uint32_t*) eight;
		s += sizeof(uint32_t);
		n -= sizeof(uint32_t);
	}

	while (n > sizeof(char)) {
		// copy in 1-byte chunks
		*(unsigned char*)string = (unsigned char) c;
		s += sizeof(char);
		n -= sizeof(char);
	}

	return s;
}


void* memcpy(void* dest, const void* src, size_t n) {
	while (n > sizeof(uint64_t)) {
		// copy in 8-byte chunks
		*(uint64_t*)dest = *(uint64_t*)src;
		n    -= sizeof(uint64_t);
		src  += sizeof(uint64_t);
		dest +=	sizeof(uint64_t);
	}
	
	while (n > sizeof(uint32_t)) {
		// copy in 4-byte chunks
		*(uint32_t*)dest = *(uint32_t*)src;
		n 	 -= sizeof(uint32_t);
		src  += sizeof(uint32_t);
		dest += sizeof(uint32_t);
	}
	
	while (n > sizeof(char)) {
		// copy in 1-byte chunks
		*(char*)dest = *(char*)src;
		n -= 	sizeof(char);
		src += 	sizeof(char);
		dest += sizeof(char);
	}

	return dest;
	
}

#define NULL (void*)0

#define true 1
#define false 0

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
	
	for(; i < n-1; i++){
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
	// silence the maximum size error in gcc
	size_t dummy = -1;
	if (dummy > PTRDIFF_MAX) {
		
	}

	return strncmp(s1, s2, dummy);
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