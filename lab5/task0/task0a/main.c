#include <stdio.h>
#include <stdlib.h>
#include "LineParser.h"
#include <linux/limits.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

void execute(cmdLine *pCmdLine){
    bool returned_error = execvp(pCmdLine->arguments[0], pCmdLine->arguments) == -1;
    if(returned_error) {
        perror("execv failed");
        exit(EXIT_FAILURE);
    }
    freeCmdLines(pCmdLine);
}

int main(int argc, char* argv[]){
    while(1) {
        char directory_path[PATH_MAX];
        getcwd(directory_path, sizeof(directory_path));
        printf("%s\n", directory_path); //print prompt
        char command[2048];
        printf("Please enter a command: ");
        fgets(command,2048, stdin); // enter command
        if(strcmp(command, "quit\n") == 0){
            exit(0);
        }
        cmdLine *cmdLine = parseCmdLines(command);
        execute(cmdLine);
    }
    return 0;
}
