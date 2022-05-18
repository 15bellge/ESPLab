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

#define DT_UNKNOWN 0
#define DT_FIFO 1
#define DT_CHR 2
#define DT_DIR 4
#define DT_BLK 6
#define DT_REG 8
#define DT_LNK 10
#define DT_SOCK 12

extern int system_call(int, ...);

typedef struct linux_dirent{
    unsigned long d_inode; /*Inode number*/
    unsigned long d_offset; /*Offset to next kinux_dirent*/
    unsigned short reclen; /*Length of this linux_dirent*/
    char d_name[]; /*Filename (null-terminated)*/
    /* length is actually (d_reclen-2-offsetof(struct linux_dirent, d_name))*/
}linux_dirent;

char* get_d_Type(char d_type){
    char * return_type;
    if(d_type == DT_UNKNOWN)
        return_type = "unknown";
    else if(d_type == DT_FIFO)
        return_type = "FIFO";
    else if(d_type == DT_CHR)
        return_type = "char device";
    else if(d_type == DT_DIR)
        return_type = "directory";
    else if(d_type == DT_BLK)
        return_type = "block device";
    else if(d_type == DT_REG)
        return_type = "regular";
    else if(d_type == DT_LNK)
        return_type = "symbolic link";
    else if(d_type == DT_SOCK)
        return_type = "socket";
    else
        return_type = "not valid";
    return return_type;
}

void debug_print_file(linux_dirent* file){
    system_call(SYS_WRITE, STDERR, "File type: ", 11);
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

void filePrint(linux_dirent* file, int debug_mode, int isPrefix, char d_type){
    int return_code_1 = system_call(SYS_WRITE, STDOUT, file->d_name, strlen(file->d_name));
    int return_code_2 = 1;
    if(isPrefix){
        system_call(SYS_WRITE, STDOUT, "\t", 1);
        return_code_2 = system_call(SYS_WRITE, STDOUT, get_d_Type(d_type), strlen(get_d_Type(d_type)));
    }
    system_call(SYS_WRITE, STDOUT, "\n", 1);
    if(return_code_1 <= 0 || return_code_2 <= 0){
        error_manager(STDOUT, "Couldn't write to STDOUT\n");
    }
    if(debug_mode) {
        debugPrint(SYS_WRITE, return_code_1);
        if (isPrefix) {
            debugPrint(SYS_WRITE, return_code_2);
        }
        debug_print_file(file);
    }
}

int main(int argc, char* argv[]){
    int directory_length;
    char buffer[8192];
    linux_dirent* file;
	int debug_mode = 0;
	int i;
    char* prefix;
    int isPrefix=0;
	for(i = 0; i < argc; i++){
		if(strcmp(argv[i], "-D") == 0){
			debug_mode = 1;
			break;
		}
        if(strncmp(argv[i], "-p", 2) == 0){
            isPrefix=1;
            prefix = argv[i]+2;
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
        char d_type = *(buffer + i + file->reclen - 1);
        if(strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0){
            if(isPrefix && strncmp(file->d_name, prefix, strlen(prefix)) ==0){
                filePrint(file, debug_mode, isPrefix, d_type);
            }
            else if(!isPrefix) {
                filePrint(file, debug_mode, isPrefix, d_type);
            }
        }
        i+=file->reclen;
    }
    system_call(SYS_CLOSE, file_descriptor);
    return 0;
}
