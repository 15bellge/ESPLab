#include "util.h"

#define SYS_WRITE 4
#define SYS_READ 3
#define SYS_EXIT 1

#define STDERR 2
#define STDOUT 1
#define STDIN 0

extern int system_call(int, ...);

void debugPrint(int id, int return_code){
	system_call(SYS_WRITE, STDOUT, "ID of system call: ", 19);
	system_call(SYS_WRITE, STDOUT, itoa(id), 2);
	system_call(SYS_WRITE, STDOUT, "\nSystem call's return code: ", 28);
	system_call(SYS_WRITE, STDOUT, itoa(return_code), 2);
	system_call(SYS_WRITE, STDOUT, "\n", 1);
}

int main(int argc, char* argv[]){
	int counter = 0;
	char curr[1];
	char prev[1] = " ";
	int write_code;
	char* to_write;
	int debug_mode = 0;
	int read_code;
	int i;
	for(i = 0; i < argc; i++){
		if(strcmp(argv[i], "-D") == 0){
			debug_mode = 1;
			break;
		}
	}

	do{
		read_code = system_call(SYS_READ, STDIN, curr, 1);
		if(debug_mode == 1){
			debugPrint(SYS_READ, read_code);
		}
		if(read_code == 0 || read_code == -1){
			system_call(SYS_EXIT, 0x55);
		}
		if(strncmp(curr, " ", 1) == 0 && strncmp(prev, " ", 1) != 0){
            if(strncmp(prev, "\n", 1) != 0){
                counter++;
            }
		}
		if(strncmp(curr, "\n", 1) == 0){
			if(strncmp(prev, " ", 1) != 0){
				counter++;
			}
			to_write = strcat(itoa(counter), "\n");
			write_code = system_call(SYS_WRITE, STDOUT, to_write, 2);
			counter = 0;
			if(debug_mode == 1){
				debugPrint(SYS_WRITE, write_code);
			}
			if(write_code == -1 || write_code == 0){
				system_call(SYS_EXIT, 0x55);
			}
		}
		prev[0] = curr[0];
	}while(write_code >= 0);
	return 0;
}
