// https://www.geeksforgeeks.org/c-program-print-contents-file/

// C program to read a file using fgetc()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  
    // Opening file
    FILE *file_ptr;

    // Character buffer that stores the read character
    // till the next iteration
    int ch;

    // Opening file in reading mode
    file_ptr = fopen("test.txt", "r");



    if (NULL == file_ptr) {
        printf("file can't be opened \n");
          return EXIT_FAILURE;
    }

    printf("Content of the file are:-: \n");

    // Printing what is written in file
    // character by character using loop.
    while ((ch = fgetc(file_ptr)) !=  (unsigned char)EOF ) {
        printf("%c", (char) ch);
    }

    // Closing the file
    fclose(file_ptr);
    return 0;
}
