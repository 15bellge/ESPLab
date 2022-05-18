#include <stdio.h>
#include <stdlib.h>
#include "LineParser.h"
#include <linux/limits.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/wait.h>

#define TERMINATED -1
#define RUNNING 1
#define SUSPENDED 0

typedef struct process {
    cmdLine *cmd; //the parsed command line
    pid_t pid; //the process id that is running the command
    int status; //status of the process: RUNNING/SUSPENDED/TERMINATED
    struct process *next; //next process in chain

} process;

process *process_list = NULL;

void addProcess(process **process_list, cmdLine *cmd, pid_t pid) {
    process *new_process = (process *) malloc(sizeof(process));
    new_process->cmd = cmd;
    new_process->pid = pid;
    new_process->status = RUNNING;
    new_process->next = *process_list;
    *process_list = new_process;
}

static char *printCMD(cmdLine *cmd) {
    char *command = cmd->arguments[0];
    for (int i = 1; i < cmd->argCount; i++) {
        strcat(command, " ");
        strcat(command, cmd->arguments[i]);
    }
    return command;
}

void printProcessList(process **process_list) {
    process *pointer = *process_list;
    while (pointer != NULL) {
        printf("%d %s %d\n", pointer->pid, printCMD(pointer->cmd), pointer->status);
        pointer = pointer->next;
    }
}

void execute(cmdLine *pCmdLine) {
    bool returned_error = execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1;
    if (returned_error) {
        perror("execv failed");
        _exit(EXIT_SUCCESS);
    }
}

void debug_print(int pid, char *command) {
    fprintf(stderr, "PID: %d\n", pid);
    fprintf(stderr, "Executing command: %s\n", command);
}

void handle_cd(bool debug_mode, char *command) {
    command[strlen(command) - 1] = '\0';
    int ch = chdir(command + 3);
    if (ch == -1) {
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
        } else if (strncmp(cmdLine->arguments[0], "cd", 2) == 0) {
            handle_cd(debug_mode, command);
        } else if (strcmp(cmdLine->arguments[0], "showprocs") == 0) {
            printProcessList(&process_list);
        } else {
            int pid = fork();
            if (debug_mode) {
                debug_print(pid, command);
            }
            if (!pid) {
                execute(cmdLine);
                exit(0);
            } else {
                addProcess(&process_list, cmdLine, pid);
            }
            if (cmdLine->blocking) {
                waitpid(pid, NULL, 0);
            }
        }
    }
    return 0;
}