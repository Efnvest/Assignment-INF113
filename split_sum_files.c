#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LENGTH 100

/*
*   Computes the sum of all the numbers in a file 
*   by dividing the workload among n child processes.
*/
int* split_sum_file(const char *filepath, int *totLines) {
    FILE *menyNumbers = fopen(filepath, "r");
    int memCap = 1000;
    int *numInMem = malloc(sizeof(int) * memCap);
    *totLines = 0;

    char buffer[MAX_LENGTH];
    while (fgets(buffer, sizeof(buffer), menyNumbers) != NULL){
        if(*totLines == memCap){
            memCap *= 2;
            numInMem = realloc(numInMem, sizeof(int) * memCap);
        }
        numInMem[*totLines] = atoi(buffer);
        (*totLines)++;
    }
    fclose(menyNumbers);
    return numInMem;
}


/**
 * Computes the sum of a portion of an array and writes the result to a file.
 *
 * Parameters:
 * - numInMem: Pointer to the array of integers.
 * - start: Starting index of the segment to sum.
 * - end: Ending index (exclusive) of the segment.
 * - childId: Unique ID for naming the output file.
 *
 * The sum is written to "partial_sum_<childId>.txt". The function then exits the process.
 * @param pointer
 * @param start 
 * @param end
 * @param pid
 */
void childSum(int *numInMem, int start, int end, int childId){
    int sum =0;
    for (int i = start; i < end; i++){
        sum += numInMem[i];
    }
    char filename[50];
    sprintf(filename, "partial_sum_%d.txt", childId);

    FILE *childFile = fopen(filename, "w");
    fprintf(childFile, "%d\n", sum);
    fclose(childFile);

    exit(0);
}

/**
 * Combines the partial sums from multiple child processes by reading files and returns the total sum.
 *
 * The function reads partial sum values from files named "partial_sum_<i>.txt" 
 * (where i is from 1 to n) and accumulates them into a total sum.
 * @param n  Number of partial sum files to read.
 * @return The total sum of all partial sums from the files.
 */
int parentSum(int n){
    int total = 0;

    for (int i = 0; i<n; i++){
        char filename[50];
        sprintf(filename, "partial_sum_%d.txt", i + 1);

        FILE *destFile = fopen(filename, "r");
        int partition;
        
        fscanf(destFile, "%d", &partition);
        total += partition;
        fclose(destFile);
    }
    return total;
}



/**
 * Main function that splits a file into parts, forks child processes to compute partial sums, 
 * and then aggregates the results.
 * argv[1]: The filepath of the file to be processed.
 * argv[2]: The number of splits/child processes (n_split).
 *
 * The program performs the following steps:
 * 1. Reads the input file and loads its contents into memory.
 * 2. Forks 'n_split' child processes, each responsible for computing the sum of a segment of the file.
 * 3. Waits for all child processes to finish.
 * 4. Aggregates the partial sums using `parentSum()` and prints the total sum.
 * 5. Frees allocated memory.
 * 
 * @param argc: Argument count, should be 3.
 * @param argv: Argument vector.
 * @return 1 or 0 if successsfull or not
 */
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <filepath> <n_split>\n", argv[0]);
        return 1;
    }

    const char *filepath = argv[1];
    int n = atoi(argv[2]);

    int totalLines;
    int *numInMem = split_sum_file(filepath, &totalLines);

    int linesPerChild = totalLines / n;
    int exLines = totalLines % n;

    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        

        if (pid == 0) {
            // Child process
            int start = i * linesPerChild + (i < exLines ? i : exLines);
            int end = start + linesPerChild + (i < exLines ? 1 : 0);
            childSum(numInMem, start, end, i + 1);
        }
    }
    //Wait for child to die
    for(int i = 0; i<n; i++){
        wait(NULL);
    }

    int totalSum = parentSum(n);
    printf("Sum: %d\n", totalSum);

    //makes the used memory usable for the rest of the system.
    free(numInMem);
    return 0;
}