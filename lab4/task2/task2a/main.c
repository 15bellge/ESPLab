#include "util.h"

#define SYS_EXIT 1
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_LSEEK 19
#define SYS_GETDENTS 141

#define SEEK_SET 0

#define STDERR 2
#define STDOUT 1
#define STDIN 0

#define O_RDONLY 0
#define O_WRONLY 1
#define O_CREAT 64

extern int system_call(int, ...);

typedef struct linux_dirent{
    unsigned long d_inode; /*Inode number*/
    unsigned long d_offset; /*Offset to next kinux_dirent*/
    unsigned short reclen; /*Length of this linux_dirent*/
    char d_name[]; /*Filename (null-terminated)*/
    /* length is actually (d_reclen-2-offsetof(struct linux_dirent, d_name))*/
}linux_dirent;

void debug_print_file(linux_dirent * file){
    system_call(SYS_WRITE, STDERR, "File name: ", 11);
    system_call(SYS_WRITE, STDERR, file->d_name, strlen(file->d_name));
    system_call(SYS_WRITE, STDERR, "\n: ", 1);
    system_call(SYS_WRITE, STDERR, "File length: ", 13);
    system_call(SYS_WRITE, STDERR, itoa(file->reclen), strlen(itoa(file->reclen)));
    system_call(SYS_WRITE, STDERR, "\n\n", 2);
}

void debugPrint(int id, int return_code){
	system_call(SYS_WRITE, STDERR, "ID of system call: ", 19);
	system_call(SYS_WRITE, STDERR, itoa(id), strlen(itoa(id)));
	system_call(SYS_WRITE, STDERR, "\nSystem call's return code: ", 28);
	system_call(SYS_WRITE, STDERR, itoa(return_code), strlen(itoa(return_code)));
	system_call(SYS_WRITE, STDERR, "\n", 1);
}

void error_manager(int output, char* message){
    system_call(SYS_WRITE, output, message, strlen(message));
    system_call(SYS_EXIT, 0x55, 0, 0);
}

int main(int argc, char* argv[]){
    int directory_length;
    char buffer[8192];
    struct linux_dirent* file;
	int debug_mode = 0;
	int i;
	for(i = 0; i < argc; i++){
		if(strcmp(argv[i], "-D") == 0){
			debug_mode = 1;
			break;
		}
	}

    int file_descriptor = system_call(SYS_OPEN, ".", O_RDONLY, 0777);
    if(debug_mode==1){
        debugPrint(SYS_OPEN, file_descriptor);
    }
    if(file_descriptor <= 0){
        error_manager(STDOUT, "Couldn't open directory");
    }
    directory_length = system_call(SYS_GETDENTS, file_descriptor, buffer, 8192);
    if(debug_mode == 1){
        debugPrint(SYS_GETDENTS, directory_length);
    }
    if(directory_length<0){
        error_manager(STDOUT, "Couldn't execute system call");
    }
    i=0;
    while(i < directory_length){
        file = (linux_dirent*)(buffer+i);
        if(strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0){
            int return_code = system_call(SYS_WRITE, STDOUT, file->d_name, strlen(file->d_name));
            system_call(SYS_WRITE, STDOUT, "\n", 1);
            if(return_code < 0){
                error_manager(STDOUT, "Couldn't write name of file");
            }
            if(debug_mode == 1){
                debugPrint(SYS_WRITE, return_code);
                debug_print_file(file);
            }
        }
        i+=file->reclen;
    }
    system_call(SYS_CLOSE, file_descriptor);
    return 0;
}
