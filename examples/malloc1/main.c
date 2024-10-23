#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char * i0 = malloc(12);
    char * i1 = malloc(12);
    char * i2 = malloc(12);
    char * i3 = malloc(12);
    
    memset(i0, 1, 12);
    memset(i1, 1, 12);
    memset(i2, 1, 12);
    memset(i3, 1, 12);
    
    free(i0);
    char* i0b = malloc(12);
    memset(i0b, 1, 12);

    free(i1);
    char* i1b = malloc(8);
    memset(i1b, 1, 12);

    free(i0b);
    free(i1b);
    free(i2);
    free(i3);

    char* a = malloc(40);
    memset(a, 1, 40);
    free(a);

    return 0;

}