#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/wait.h>
#include "LineParser.h"

#define EXIT_FAILURE 1
#define MAX_SIZE 2048

#define STDOUT 1
#define STDIN 0

void execute(cmdLine *pCmdLine) {
    bool returned_error = execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1;
    if (returned_error) {
        perror("execv failed");
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    bool debug_mode = false;
    for (int i = 0; i < argc; i++) {
        if (strncmp(argv[i], "-d", 2) == 0) {
            debug_mode = true;
        }
    }
    int file_descriptors[2];
    if (pipe(file_descriptors) < 0) {
        exit(EXIT_FAILURE);
    }
    if (debug_mode) {
        perror("parent_process>forking...");
    }
    int pid1 = fork();
    if (debug_mode && pid1) {
        fprintf(stderr, "parent_process>created first process with id: %d\n", pid1);
    }
    if (!pid1) {//child1
        if (debug_mode) {
            perror("child1>redirecting stdout to the write-end of the pipe..");
        }
        close(STDOUT);//close stdout
        int write_descriptor = dup(file_descriptors[1]);//duplicate write-end
        close(write_descriptor);
        char *command = "ls -l";
        if (debug_mode) {
            fprintf(stderr, "child1>going to execute cmd: %s\n", command);
        }
        cmdLine *cmdLine = parseCmdLines(command);
        execute(cmdLine);
        exit(0);
    }
    else{
        if (debug_mode) {
            perror("parent_process>closing the write end of the pipe...");
        }
        close(file_descriptors[1]);//close write-end
        int pid2 = fork();
        if (debug_mode && pid2) {
            fprintf(stderr, "parent_process>created second process with id: %d\n", pid2);
        }
        if (!pid2) {//child2
            if (debug_mode) {
                perror("child2>redirecting stdin to the read-end of the pipe...");
            }
            close(STDIN);//close stdin
            int read_descriptor = dup(file_descriptors[0]);//duplicate read-end
            close(read_descriptor);
            char *command = "tail -n 2";
            if (debug_mode) {
                fprintf(stderr, "child2>going to execute cmd: %s\n", command);
            }
            cmdLine *cmdLine = parseCmdLines(command);
            execute(cmdLine);
            exit(0);
        }
        else {
            if (debug_mode) {
                perror("parent_process>closing the read end of the pipe...");
            }
            close(file_descriptors[0]);//close read-end
            if (debug_mode) {
                perror("parent_process>waiting for child processes to terminate...");
            }
            //int status;
            //int pid_returned = wait(&status);
            wait(NULL);
            wait(NULL);
            //printf("status1: %d, pid_returned1: %d", status, pid_returned);
            //pid_returned=wait(&status);
            //printf("status2: %d, pid_returned2: %d", status, pid_returned);
            if (debug_mode) {
                perror("parent_process>exiting...");
            }
            exit(0);
        }
    }
    return 0;
}
