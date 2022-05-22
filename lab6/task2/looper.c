#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

void handler(int signum);

int main(int argc, char **argv) {

    printf("Starting the program\n");
    signal(SIGINT, handler);
    signal(SIGTSTP, handler);
    signal(SIGCONT, handler);
    while (1) {
        sleep(2);
    }

    return 0;
}

void handler(int signum){
    signal(signum, SIG_DFL);
    raise(signum);

    if (signum == SIGCONT) {
        signal(SIGTSTP, handler);
    }
    if (signum == SIGTSTP) {
        signal(SIGCONT, handler);
    }
}
