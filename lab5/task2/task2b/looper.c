#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

void handler(int signum) {
    printf("Signal received: %s\n", strsignal(signum));
    signal(signum, SIGDFL);
    raise(signum);
    if (signum == SIGCONT) {
        signal(SIGTSTP, handler);
    }
    if (signum == SIGTSTP) {
        signal(SIGCONT, handler);
    }
}

int main(int argc, char **argv) {

    printf("Starting the program\n");
    signal(SIGTSTP, handler);
    signal(SIGINT, handler);
    signal(SIGTSTP, handler);
    while (1) {
        sleep(2);
    }

    return 0;
}