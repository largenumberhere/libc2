#include <stdio.h>

int main(int argc, char*argv[]) {
    printf("hello!\n");
    
    for(int i = 0; i < argc; i++){
        printf("arg '%i' recieved '%s'\n", i, argv[i]);
    }

    printf("goodbye\n");

    return 0;   
}