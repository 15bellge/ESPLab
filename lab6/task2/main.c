#include <stdio.h>
#include <stdlib.h>
#include "LineParser.h"
#include <linux/limits.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/time.h>
#include <fcntl.h>

#define TERMINATED -1
#define RUNNING 1
#define SUSPENDED 0

#define STDOUT 1
#define STDIN 0

#define EXIT_FAILURE 1

#define MAX_SIZE 2048

typedef struct process {
    cmdLine *cmd; //the parsed command line
    pid_t pid; //the process id that is running the command
    int status; //status of the process: RUNNING/SUSPENDED/TERMINATED
    int start;
    int end;
    struct process *next; //next process in chain

} process;

process *process_list = NULL;
struct timeval now;
int file_descriptors[2];
int input_file, output_file;

void addProcess(process **process_list, cmdLine *cmd, int start, pid_t pid) {
    process *new_process = (process *) malloc(sizeof(process));
    new_process->cmd = cmd;
    new_process->pid = pid;
    new_process->status = RUNNING;
    new_process->start = start;
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
        int time = -1;
        if (pointer->end == -1) {
            gettimeofday(&now, NULL);
            time = now.tv_sec - pointer->start;
        } else {
            time = pointer->end - pointer->start;
        }
        printf("%d\t%s\t%d\t%d\n", pointer->pid, command, pointer->status, time);
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
        //printf("kill - %d\n", kil);
        if (kil == -1) {
            perror("nap suspemd failed\n");
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

void updateEnd(int pid, int end) {
    process *pointer = process_list;
    while (pointer != NULL) {
        if (pointer->pid == pid) {
            pointer->end = end;
            break;
        }
        pointer = pointer->next;
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

void single_execution(int debug_mode, cmdLine *cmd, char* command) {
    int end = -1;
    gettimeofday(&now, NULL);
    int start = now.tv_sec;
    int pid = fork();
    if (debug_mode) {
        debug_print(pid, command);
    }
    if (!pid) {
        IO_redirect(cmd);
        execute(cmd);
        gettimeofday(&now, NULL);
        end = now.tv_sec;
        updateEnd(pid, end);
        exit(0);
    } else {
        addProcess(&process_list, cmd, start, pid);
    }
    if (cmd->blocking) {
        waitpid(pid, NULL, 0);
    }
}

void pipeline_execution(int debug_mode, cmdLine *cmd, char* command) {
    if (pipe(file_descriptors) < 0) {
        exit(EXIT_FAILURE);
    }
    char* first_command = strtok(command, "|");
    char* second_command = strtok(NULL, "|");
    if (debug_mode) {
        perror("parent_process>forking...");
    }
    int end = -1;
    gettimeofday(&now, NULL);
    int start = now.tv_sec;
    int pid1 = fork();
    if (debug_mode && pid1) {
        fprintf(stderr, "parent_process>created first process with id: %d\n", pid1);
    }
    if (!pid1) {//child1
        IO_redirect(cmd);
        if (debug_mode) {
            perror("child1>redirecting stdout to the write-end of the pipe..");
        }
        close(STDOUT);//close stdout
        dup(file_descriptors[1]);//duplicate write-end
        close(file_descriptors[1]);
        if (debug_mode) {
            fprintf(stderr, "child1>going to execute cmd: %s\n", first_command);
        }
        execute(cmd);
        gettimeofday(&now, NULL);
        end = now.tv_sec;
        updateEnd(pid1, end);
        exit(0);
    } else {
        addProcess(&process_list, cmd, start, pid1);
        if (debug_mode) {
            perror("parent_process>closing the write end of the pipe...");
        }
        close(file_descriptors[1]);//close write-end
        int pid2 = fork();
        if (debug_mode && pid2) {
            fprintf(stderr, "parent_process>created second process with id: %d\n", pid2);
        }
        if (!pid2) {//child2
            IO_redirect(cmd->next);
            if (debug_mode) {
                perror("child2>redirecting stdin to the read-end of the pipe...");
            }
            close(STDIN);//close stdin
            dup(file_descriptors[0]);//duplicate read-end
            close(file_descriptors[0]);
            if (debug_mode) {
                fprintf(stderr, "child2>going to execute cmd: %s\n", second_command);
            }
            execute(cmd->next);
            gettimeofday(&now, NULL);
            end = now.tv_sec;
            updateEnd(pid2, end);
            exit(0);
        } else {
            addProcess(&process_list, cmd->next, start, pid2);
            if (debug_mode) {
                perror("parent_process>closing the read end of the pipe...");
            }
            close(file_descriptors[0]);//close read-end
            if (debug_mode) {
                perror("parent_process>waiting for child processes to terminate...");
            }
            wait(NULL);
            wait(NULL);
            if (debug_mode) {
                perror("parent_process>exiting...");
            }
            exit(0);
        }
    }
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
        fgets(command, 2048, stdin); // enter command
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