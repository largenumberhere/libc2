#include <string.h>
#include <stdio.h>
#include <stdlib.h>


int main() {
    char* string = "hello";

    if (strlen(string) != 5) {
        printf("strlen returned wrong result\n");
        return 1;
    }

    return 0;
    
}