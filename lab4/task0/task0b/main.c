#include "util.h"

#define SYS_OPEN 5
#define SYS_LSEEK 19
#define SYS_WRITE 4
#define SYS_CLOSE 6
#define SYS_EXIT 1

#define O_RDRW 2

#define SEEK_SET 0

#define STDIN 0
#define STDOUT 1
#define STDERR 2
extern int system_call(int, ...);

int main(int argc, char* argv[], char* envp[]){
	char* x_name = argv[2];
	char* file_name = argv[1];
	int file_descriptor;
	while(strlen(x_name) < 5){
		strcat(x_name, " ");
	}
	if ((file_descriptor = system_call(SYS_OPEN, file_name, O_RDRW, 0644)) == -1){
		system_call(SYS_EXIT, 0x55);
	}
	if (system_call(SYS_LSEEK, file_descriptor, 0x506, SEEK_SET) == 0){
		system_call(SYS_EXIT, 0x55);
	}
	if (system_call(SYS_WRITE, file_descriptor, x_name, strlen(x_name)) <= 0){
		system_call(SYS_EXIT, 0x55);
	}
	if (system_call(SYS_CLOSE, file_descriptor) == -1){
		system_call(SYS_EXIT, 0x55);
	}
	return 0;
}
