#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int file_descriptors[2] = {0};

    if(pipe(file_descriptors)!=0) {
        perror("failed to create pipe");
        exit(1);
    }


    dprintf(file_descriptors[1], "hello, world!\n");
    char tmp[1024] = {0};
    
    
    read(file_descriptors[0],tmp, 1023);
    fprintf(stdout, "mrow\n");
    
    printf("recieved: '%s'", tmp);


}