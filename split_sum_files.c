#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LENGTH 100

/*
    * Function: split_sum_file
    * ----------------------------
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