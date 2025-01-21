#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


typedef struct {
    int in_fd;
    int out_fd;
} PipeFDs;

PipeFDs new_pipe() {
    int file_descriptors[2] = {0};
    if(pipe(file_descriptors)!=0) {
        perror("failed to create pipe");
        exit(1);
    }

    PipeFDs pipe =  {
        .in_fd = file_descriptors[1],
        .out_fd = file_descriptors[0]
    };

    return pipe;
}

void close_pipe(PipeFDs pipe) {
    close(pipe.in_fd);
    close(pipe.out_fd);
}

int main() { 
    // format string
    PipeFDs pipe = new_pipe();
    dprintf(pipe.in_fd, "hello, world!\n");
    char tmp[1024];
    memset(tmp, 0, sizeof(tmp));
    read(pipe.out_fd, tmp, sizeof(tmp)-1);
    if (strcmp(tmp, "hello, world!\n") !=0) {
        perror("dprintf gave incorrect output");
        exit(1);
    }

    // %s
    dprintf(pipe.in_fd, "%s!\n", "mrrrp");
    memset(tmp, 0, sizeof(tmp));
    read(pipe.out_fd, tmp, sizeof(tmp)-1);
    if (strcmp(tmp, "mrrrp!\n") !=0) {
        perror("dprintf gave incorrect output");
        exit(1);
    }

    // %i

    close_pipe(pipe);
    pipe = new_pipe();
    dprintf(pipe.in_fd, "aa%i--------+--------+", 12345);
    memset(tmp, 0, sizeof(tmp));
    read(pipe.out_fd, tmp, sizeof(tmp)-1);
    if (strcmp(tmp, "aa12345--------+--------+") !=0) {
        printf(tmp);
        perror("dprintf gave incorrect output");
        exit(1);
    }

    // %d
    dprintf(pipe.in_fd, "%d-1", 5678);
    memset(tmp, 0, sizeof(tmp));
    read(pipe.out_fd, tmp, sizeof(tmp)-1);
    if (strcmp(tmp, "5678-1") !=0) {
        perror("dprintf gave incorrect output");
        exit(1);
    }

    // %llx
    dprintf(pipe.in_fd, "~%llx~", (size_t) 0xFFFFFFFFFFFFFFFF);
    memset(tmp, 0, sizeof(tmp));
    read(pipe.out_fd, tmp, sizeof(tmp)-1);
    if ((strcmp(tmp, "~0xffffffffffffffff~") !=0) && (strcmp(tmp, "~ffffffffffffffff~")!=0)) {
        printf("'%s'", tmp);
        perror("dprintf gave incorrect output");
        exit(1);
    }


    close_pipe(pipe);



}