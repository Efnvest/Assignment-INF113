#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LENGTH 100

/*
    * Function: split_sum_file
    * ----------------------------
    *   Reads the numbers from the file and stores them in dynamically allocated memory.
    *   Returns the array of numbers and sets totalLines to the total number of lines in the file.
*/
int* split_sum_file(const char *filepath, int *totLines) {
    FILE *menyNumbers = fopen(filepath, "r");
    if (!menyNumbers) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    int memCap = 1000;
    int *numInMem = malloc(sizeof(int) * memCap);
    *totLines = 0;

    char buffer[MAX_LENGTH];
    while (fgets(buffer, sizeof(buffer), menyNumbers) != NULL) {
        if (*totLines == memCap) {
            memCap *= 2;
            numInMem = realloc(numInMem, sizeof(int) * memCap);
        }
        numInMem[*totLines] = atoi(buffer);
        (*totLines)++;
    }
    fclose(menyNumbers);
    return numInMem;
}

/*
    * Function: childSum
    * ----------------------------
    *   Each child process computes the sum of its assigned range of numbers and writes it to a file.
*/
void childSum(int *numInMem, int start, int end, int childId) {
    int sum = 0;
    for (int i = start; i < end; i++) {
        sum += numInMem[i];
    }
    char filename[50];
    sprintf(filename, "partial_sum_%d.txt", childId);  // Create a unique file for each child

    FILE *fileOut = fopen(filename, "w");
    if (fileOut == NULL) {
        perror("Error creating partial sum file");
        exit(EXIT_FAILURE);
    }
    fprintf(fileOut, "%d\n", sum);
    fclose(fileOut);

    exit(0);  // End the child process
}

/*
    * Function: parentSum
    * ----------------------------
    *   The parent process collects results from partial sum files created by children and computes the total sum.
*/
int parentSum(int n) {
    int total = 0;

    for (int i = 0; i < n; i++) {
        char filename[50];  // Make sure the buffer is large enough for the filename
        sprintf(filename, "partial_sum_%d.txt", i + 1);  // Include .txt extension

        FILE *destFile = fopen(filename, "r");
        if (destFile == NULL) {
            perror("Error opening partial sum file");
            exit(EXIT_FAILURE);
        }

        int partition;
        fscanf(destFile, "%d", &partition);  // Read the sum from the file
        total += partition;
        fclose(destFile);
    }
    return total;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <filepath> <n_split>\n", argv[0]);
        return 1;
    }

    const char *filepath = argv[1];
    int n = atoi(argv[2]);

    // Read the numbers from the file
    int totalLines;
    int *numInMem = split_sum_file(filepath, &totalLines);  // Use int* for numInMem

    // Calculate how many lines each child process should handle
    int linesPerChild = totalLines / n;
    int exLines = totalLines % n;  // Distribute extra lines if needed

    // Fork child processes
    for (int i = 0; i < n; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process
            int start = i * linesPerChild + (i < exLines ? i : exLines);
            int end = start + linesPerChild + (i < exLines ? 1 : 0);
            childSum(numInMem, start, end, i + 1);
        }
    }

    // Parent process waits for all child processes to finish
    for (int i = 0; i < n; i++) {
        wait(NULL);  // Wait for child processes to finish
    }

    // Parent collects the partial sums and computes the total sum
    int totalSum = parentSum(n);
    printf("Sum: %d\n", totalSum);

    // Free the allocated memory
    free(numInMem);

    return 0;
}
