#include <stdio.h>
#include <stdlib.h>
#include "LineParser.h"
#include "pipeFunctions.h"
#include <linux/limits.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#define TERMINATED -1
#define RUNNING 1
#define SUSPENDED 0

#define STDIN 0
#define STDOUT 1

#define EXIT_FAILURE 1

#define MAX_SIZE 2048

typedef struct process {
    cmdLine *cmd; //the parsed command line
    pid_t pid; //the process id that is running the command
    int status; //status of the process: RUNNING/SUSPENDED/TERMINATED
    int end;
    struct process *next; //next process in chain

} process;

//typedef struct command {
//    char name[MAX_SIZE];//name of command
//    struct command *next;//next command
//} command;

process *process_list = NULL;
//command command_list = NULL;
int input_file, output_file;
int **pipes;

void addProcess(process **process_list, cmdLine *cmd, pid_t pid) {
    process *new_process = (process *) malloc(sizeof(process));
    new_process->cmd = cmd;
    new_process->pid = pid;
    new_process->status = RUNNING;
    new_process->end = -1;
    new_process->next = *process_list;
    *process_list = new_process;
}

static char *printCMD(cmdLine *cmd) {
    char *command = malloc(50);
    strcpy(command, cmd->arguments[0]);
    for (int i = 1; i < cmd->argCount; i++) {
        strcat(command, " ");
        strcat(command, cmd->arguments[i]);
    }
    return command;
}

void freeProcessList(process *process_list) {
    process *temp;
    while (process_list != NULL) {
        temp = process_list;
        process_list = process_list->next;
        freeCmdLines(temp->cmd);
        free(temp);
    }
}

void updateProcessStatus(process *process_list, int pid, int status) {
    process *temp = process_list;
    while (temp != NULL) {
        if (temp->pid == pid) {
            temp->status = status;
            break;
        }
        temp = temp->next;
    }
}

void updateProcessList(process *process_list) {
    process *temp = process_list;
    int status;
    int result_waitpid;
    while (temp != NULL) {
        result_waitpid = waitpid(temp->pid, &status, WNOHANG | WUNTRACED | WCONTINUED);
        if (result_waitpid > 0) {
            if (WIFEXITED(status)) {
                updateProcessStatus(temp, temp->pid, TERMINATED);
            } else if (WIFSIGNALED(status)) {
                updateProcessStatus(temp, temp->pid, TERMINATED);
            } else if (WIFSTOPPED(status)) {
                updateProcessStatus(temp, temp->pid, SUSPENDED);
            } else if (WIFCONTINUED(status)) {
                updateProcessStatus(temp, temp->pid, RUNNING);
            }
        } else if (result_waitpid == -1) {
            updateProcessStatus(temp, temp->pid, TERMINATED);
        }
        temp = temp->next;
    }
}

void printProcessList(process **process_list) {
    process *pointer = *process_list;
    updateProcessList(pointer);
    printf("PID\tCOMMAND\t\tSTATUS\tTIME\n");
    process *last_process = NULL;
    while (pointer != NULL) {
        char *command = printCMD(pointer->cmd);
        printf("%d\t%s\t%d\n", pointer->pid, command, pointer->status);
        free(command);
        if (pointer->status == -1) {
            if (last_process == NULL) {
                (*process_list) = (*process_list)->next;
            } else {
                last_process->next = pointer->next;
            }
        } else {
            last_process = pointer;
        }
        pointer = pointer->next;
    }
}

void execute(cmdLine *pCmdLine) {
    bool returned_error = execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1;
    if (returned_error) {
        perror("execv failed");
        exit(0);
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
}

void nap(int time, int pid) {
    int id = fork();
    if (!id) {
        int kil = kill(pid, SIGTSTP);
        if (kil == -1) {
            perror("nap suspend failed\n");
        } else {
            printf("nap suspend succeeded\n");
        }
        sleep(time);
        kil = kill(pid, SIGCONT);
        if (kil == -1) {
            perror("nap continue failed\n");
        } else {
            printf("nap continue succeeded\n");
        }
    }
}

void stop(int pid) {
    int kil = kill(pid, SIGINT);
    if (kil == -1) {
        perror("stop failed\n");
    } else {
        printf("stop succeeded\n");
    }
}

void IO_redirect(cmdLine *cmd) {
    if (cmd->inputRedirect) {
        close(STDIN);
        input_file = open(cmd->inputRedirect, O_RDONLY);
    }
    if (cmd->outputRedirect) {
        close(STDOUT);
        output_file = open(cmd->outputRedirect, O_WRONLY);
    }
}

void single_execution(int debug_mode, cmdLine *cmd, char *command) {
    int pid = fork();
    if (debug_mode) {
        debug_print(pid, command);
    }
    if (!pid) {
        IO_redirect(cmd);
        execute(cmd);
        exit(0);
    } else {
        addProcess(&process_list, cmd, pid);
    }
    if (cmd->blocking) {
        waitpid(pid, NULL, 0);
    }
}

////counts commands
int count_commands(cmdLine *cmd) {
    int numberOfCommands = 0;
    cmdLine *cmd_pointer = cmd;
    while (cmd_pointer != NULL) {
        numberOfCommands++;
        cmd_pointer = cmd_pointer->next;
    }
    return numberOfCommands;
}

void pipeline_execution(int debug_mode, cmdLine *cmd, char *command) {
    int number_of_commands = count_commands(cmd);
    pipes = createPipes(number_of_commands - 1);////create pipes
    fprintf(stderr, "numberOfPipe: %d\n", number_of_commands - 1);
    int *right_pipe;
    int *left_pipe;
    for (int i = 0; i < number_of_commands; i++) {
        right_pipe = rightPipe(pipes, cmd);
        left_pipe = leftPipe(pipes, cmd);
        int pid = fork();
        if (!pid) {////child
            if (i > 0) {
                close(left_pipe[1]);////close write-end of "in" pipe
                dup2(left_pipe[0], STDIN_FILENO);////dup read-end of "in" pipe
                close(left_pipe[0]);////close read-end of "in" pipe
            }
            if (i < number_of_commands - 1) {
                close(right_pipe[0]);////close read-end of "out" pipe
                dup2(right_pipe[1], STDOUT_FILENO);////dup write-end of "out" pipe
                close(right_pipe[1]);////close write-end of "out" pipe
            }
            execute(cmd);
            exit(0);
        } else {////parent
            if (i > 0) {////closing "in" pipe because it will no longer be used
                close(left_pipe[0]);
                close(left_pipe[1]);
            }
        }
        cmd = cmd->next;
    }
    close(left_pipe[0]);
    close(left_pipe[1]);
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
            _exit(3);
        }
        char command[MAX_SIZE];
        fgets(command, MAX_SIZE, stdin); // enter command
        cmdLine *cmdLine = parseCmdLines(command);
        if (strcmp(cmdLine->arguments[0], "quit") == 0) {
            _exit(3);
        } else if (strncmp(cmdLine->arguments[0], "cd", 2) == 0) {
            handle_cd(debug_mode, command);
        } else if (strcmp(cmdLine->arguments[0], "showprocs") == 0) {
            printProcessList(&process_list);
        } else if (strcmp(cmdLine->arguments[0], "nap") == 0) {
            int time = atoi(cmdLine->arguments[1]);
            int pid1 = atoi(cmdLine->arguments[2]);
            nap(time, pid1);
        } else if (strcmp(cmdLine->arguments[0], "stop") == 0) {
            int pid_to_kill = atoi(cmdLine->arguments[1]);
            stop(pid_to_kill);
        } else if (cmdLine->next == NULL) {
            single_execution(debug_mode, cmdLine, command);
        } else {
            pipeline_execution(debug_mode, cmdLine, command);
        }
    }
    return 0;
}