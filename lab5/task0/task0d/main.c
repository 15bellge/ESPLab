#include <stdio.h>
#include <stdlib.h>
#include "LineParser.h"
#include <linux/limits.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/wait.h>

void execute(cmdLine *pCmdLine) {
    bool returned_error = execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1;
    if (returned_error) {
        perror("execv failed");
        _exit(EXIT_SUCCESS);
    }
    freeCmdLines(pCmdLine);
}

void debug_print(int pid, char *command) {
    fprintf(stderr, "PID: %d\n", pid);
    fprintf(stderr, "Executing command: %s\n", command);
}

void handle_cd(bool debug_mode, char* command){
    command[strlen(command)-1] = '\0';
    int ch = chdir(command + 3);
    if (ch == -1){
        perror("chdir failed");
    }
    if (debug_mode) {
        perror("chdir result");
    }
    //getcwd(directory_path, sizeof(directory_path)); //update prompt
    //printf("SECOND: %s\n", directory_path); //print prompt
}

int main(int argc, char *argv[]) {
    bool debug_mode = false;
    for (int i = 0; i < argc; i++) {
        if (strncmp(argv[i], "-d", 2) == 0) {
            debug_mode = true;
        }
    }
    while (1) {
        char directory_path[PATH_MAX];
        getcwd(directory_path, sizeof(directory_path));
        if (getcwd(directory_path, sizeof(directory_path)) == NULL) {
            _exit(0);
        }
        printf("%s\n", directory_path); //print prompt
        char command[2048];
        printf("Please enter a command: ");
        fgets(command, 2048, stdin); // enter command
        cmdLine *cmdLine = parseCmdLines(command);
        if (strcmp(cmdLine->arguments[0], "quit") == 0) {
            _exit(0);
        } else if (strncmp(command, "cd", 2) == 0) {
            handle_cd(debug_mode, command);
        } else {
            int pid = fork();
            if (!pid) {
                execute(cmdLine);
                _exit(0);
            }
            if (debug_mode) {
                debug_print(pid, command);
            }
        }
    }
    return 0;
}