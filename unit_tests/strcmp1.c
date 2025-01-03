#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char str1[] = "abcd";
    char str2[] = "abCd";
    char str3[] = "abcd";

    if (strcmp(str1, str2) !=32) {
        printf("str1 should not be equal to str2\n");
        goto exit_err;        
    }
    else if (strcmp(str2, str3) == 32) {
        printf("str2 should not be equal to str3\n");
        goto exit_err;        
    } else if (strcmp(str1, str3) !=0) {
        printf("str1 should be equal to str3\n");
        goto exit_err;        
    }

    return 0;

exit_err:
    exit(1);

}