#define size_t unsigned long long

void memset(void* s, int c, size_t n){
	for(int i = 0; i < n; i++){
		*(char*)s = (unsigned char) c;
	}
}


void memcpy(void* dest, const void* src, size_t n) {
	for(size_t i = 0; i < n; i++){
		char* destc = (char*) dest;
		char* srcc = (char*) src;

		destc[i] =  srcc[i];
	}
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
	return strncmp(s1, s2, (size_t)-1);
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