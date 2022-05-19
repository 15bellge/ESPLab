#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define EXIT_FAILURE 1

int main(int argc, char* argv[]){
    int file_descriptors[2];
    if(pipe(file_descriptors) < 0){
        exit(EXIT_FAILURE);
    }
    char input_buffer[6];
    int pid = fork();
    if(!pid){//child
        write(file_descriptors[1], "hello\n", 6);
        exit(2);
    }
    else{//parent
        read(file_descriptors[0], input_buffer, 6);
        printf("%s", input_buffer);
    }
    return 0;
}
