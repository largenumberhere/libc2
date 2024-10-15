#include <unistd.h> 
#include <stdio.h>

int main (){
	unsigned long brk_val = sbrk(1);
	printf("%i", brk_val);
	return 0;
}
