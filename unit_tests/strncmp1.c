#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int main() {
    char str1[] = "abcd";
    char str2[] = "abCd";
    char str3[] = "abcd";

    
    if (strncmp(str1, str2, 4) !=32) {
        // check for behaviour same as strcmp when n=strlen(var)
        printf("str1 should not be equal to str2\n");
        goto exit_err;        
    }
    if (strncmp(str1, str2, 3) != 32) {
        // check strnmp handles first 3 characters correctly for different
        printf("str and str2 should differ by 32 at the 3rd character!\n");
        goto exit_err;
    }
    else if (strncmp(str2, str3, 4) == 32) {
        // check strncmp handles first 4 characters correctly for different at pos3
        printf("str2 should not be equal to str3\n");
        goto exit_err;        
    } else if (strncmp(str1, str3, 4) !=0) {
        // check strncmp handles first 4 characters correctly for equal
        printf("str1 should be equal to str3\n");
        goto exit_err;        
    }

    if (strncmp(str1, str2, 2) !=0) {
        // check strncmp handles first 2 characters correctly
        printf("fitst 2 characters of str1 should be equal to str3\n");
        goto exit_err;
    }


    int32_t large_number = -1;
    if (strncmp(str1, str2, large_number) != 32) {
        // check for strlen acting properly when n is past the null-terminator
        printf("strncmp has flown off into space!\n");
        goto exit_err;
    }


    return 0;

exit_err:
    exit(1);


}