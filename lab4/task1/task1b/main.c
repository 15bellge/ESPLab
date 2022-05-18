#include "util.h"

#define SYS_EXIT 1
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6

#define STDERR 2
#define STDOUT 1
#define STDIN 0

#define O_RDONLY 0
#define O_WRONLY 1
#define O_CREAT 64

extern int system_call(int, ...);

void debugPrint(int id, int return_code){
	system_call(SYS_WRITE, STDERR, "ID of system call: ", 19);
	system_call(SYS_WRITE, STDERR, itoa(id), 2);
	system_call(SYS_WRITE, STDERR, "\nSystem call's return code: ", 28);
	system_call(SYS_WRITE, STDERR, itoa(return_code), 2);
	system_call(SYS_WRITE, STDERR, "\n", 1);
}

void error_manager(int output, char* message){
    system_call(SYS_WRITE, output, message, strlen(message));
    system_call(SYS_EXIT, 0x55, 0, 0);
}

int main(int argc, char* argv[]){
	int counter = 0;
    int space_counter = 0;
	char curr[1];
	char prev[1] = " ";
	int write_code;
	char* to_write;
	int debug_mode = 0;
	int read_code;
	int i;
    int input = STDIN;
    int output = STDOUT;
	for(i = 0; i < argc; i++){
		if(strcmp(argv[i], "-D") == 0){
			debug_mode = 1;
			break;
		}
        if(strncmp(argv[i], "-i", 2) == 0) {
            int input_file = system_call(SYS_OPEN, argv[i] + 2, O_RDONLY, 0777);
            if (input_file >= 0) {
                input = input_file;
            } else {
                error_manager(STDOUT, "Input file couldn't be open");
            }
        }
        if(strncmp(argv[i], "-o", 2) == 0){
            int output_file = system_call(SYS_OPEN, argv[i]+2, O_WRONLY | O_CREAT, 0777 );
            if(output_file >= 0){
                output = output_file;
            }
            else {
                error_manager(STDOUT, "Output file couldn't be open");
            }
        }
	}

    if(debug_mode == 1){
        system_call(SYS_WRITE, STDERR, "input: ", 7);
        if(input!=0) {
            system_call(SYS_WRITE, STDERR, strcat(itoa(input),"\n"), 3);
        }
        else{
            system_call(SYS_WRITE, STDERR, "input\n", 6);
        }
        system_call(SYS_WRITE, STDERR, "output: ", 8);
        if(output!=1) {
            system_call(SYS_WRITE,STDERR, strcat(itoa(output), "\n"), 3);
        }
        else{
            system_call(SYS_WRITE, STDERR, "output\n", 7);
        }
    }

	do{
		read_code = system_call(SYS_READ, input, curr, 1);
		if(debug_mode == 1){
			debugPrint(SYS_READ, read_code);
		}
		if(read_code == 0 || read_code == -1){
			system_call(SYS_EXIT, 0x55);
		}
        if(strncmp(curr, " ", 1) == 0){
            space_counter++;
        }
		if(strncmp(curr, " ", 1) == 0 && strncmp(prev, " ", 1) != 0) {
            if (strncmp(prev, "\n", 1) != 0) {
                counter++;
            }
		}
		if(strncmp(curr, "\n", 1) == 0){
			if(strncmp(prev, " ", 1) != 0){
				counter++;
			}
			to_write = strcat(itoa(counter), "\n");
            system_call(SYS_WRITE, output, "word counter: ", 14);
			write_code = system_call(SYS_WRITE, output, to_write, 2);
			counter = 0;
            strcat(itoa(space_counter), "\n");
            system_call(SYS_WRITE, output, "space counter: ", 15);
            system_call(SYS_WRITE, output, to_write, 2);
            space_counter=0;
			if(debug_mode == 1){
				debugPrint(SYS_WRITE, write_code);
			}
			if(write_code == -1 || write_code == 0){
				system_call(SYS_EXIT, 0x55);
			}
		}
		prev[0] = curr[0];
	}while(write_code >= 0);

    if(strncmp(argv[i], "-i", 2) == 0){
        system_call(SYS_CLOSE, input);
    }

    if(strncmp(argv[i], "-o", 2) == 0){
        system_call(SYS_CLOSE, output);
    }
	return 0;
}
