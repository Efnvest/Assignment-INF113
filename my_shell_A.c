#include <stdio.h>  // getline
#include <stdlib.h> // exit
#include <string.h> // strtok
#include <unistd.h> // execvp
#include <errno.h>  // errno for error handling
#include <sys/wait.h> // wait

#define MAX_ARGS 256

size_t n = 0;
char *line = NULL;
char *args[MAX_ARGS];

/*
    Reads one line from the standard input and
    splits it into tokens. The resulting tokens
    are stored args, terminated by a NULL pointer.
*/
void parse_command_from_user()
{
    int rc = getline(&line, &n, stdin);
    if (rc < 0) // Close the shell at end-of-input
        exit(0);

    int i = 0;
    args[i++] = strtok(line, " \n");
    while (i < MAX_ARGS && args[i - 1] != NULL)
        args[i++] = strtok(NULL, " \n");


}

int main()
{
    while(1){
        printf("> ");

        parse_command_from_user();

        if (args[0] == NULL) {
            continue;
        }
        
        //create child
        pid_t pid = fork();

        if (pid < 0) {
            // If fork fails
            perror("Fork failed");
            exit(1);
        }
        else if (pid == 0) {
            // Child process: Execute the command
            if (execvp(args[0], args) < 0) {
                perror("Error executing command");
            }
            exit(1);  //Exit child process
        } else {
            //Wait for the child process to finish
            int status;
            waitpid(pid, &status, 0);  // Wait for child complete
        }

    }
}