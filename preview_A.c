#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 1024

/*
    * Function: preview
    * ----------------------------
    *   Opens a file and prints the first n lines of the file.
    *
    *   filepath: The path to the file to preview.
    *   n: The number of lines to print.
    *
    *   returns: void
    */
void preview(const char *filepath, int n) {
    FILE * view = fopen(filepath, "r");
     

    char line[MAX_LINE_LENGTH];  
    
    int count = 0;

    while (fgets(line, sizeof(line), view) != NULL && count < n) {
        printf("%s", line);
        count++;
    }

    fclose(view);
}


int main(int argc, char *argv[]) {
    
    if (argc != 3) {
        printf("Usage: %s <filepath> <output_lines>\noutput_lines should be integer and larger than 0.", argv[0]);
        return 1;
    }
    int n = atoi(argv[2]);  // atoi() converts a string to an integer

    preview(argv[1], n);
    return 0;
}