#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <elf.h>
#include <fcntl.h>
#include <unistd.h>

struct fun_desc {
    char *name;

    void (*fun)();
};

bool debug_mode = false;
int curr_file = -1;
Elf32_Ehdr *elf_file;
struct stat fd;
void *map;

void toggle_debug_mode() {
    if (debug_mode) {
        debug_mode = false;
        printf("%s", "Debug flag now off\n");
    } else {
        debug_mode = true;
        printf("%s", "Debug flag now on\n");
    }
}

void examine_ELF_file() {
    char file_name[100];
    char file[100];
    printf("please enter file name : ");
    fgets(file, 99, stdin);
    file[strlen(file) - 1] = '\0';
    strncpy(file_name, file, 100);
    if (curr_file != -1) {
        printf("in current file = -1 %d", curr_file);
        close(curr_file);
    }
    curr_file = open(file_name, O_RDONLY);
    fstat(curr_file, &fd);
    map = mmap(NULL, fd.st_size, PROT_READ, MAP_SHARED, curr_file, 0);
    elf_file = (Elf32_Ehdr *) map;
    printf("magic 1 2 3 : %c %c %c\n", elf_file->e_ident[EI_MAG1], elf_file->e_ident[EI_MAG2],
           elf_file->e_ident[EI_MAG3]);
    if (elf_file->e_ident[EI_MAG1] != 'E' || elf_file->e_ident[EI_MAG2] != 'L' || elf_file->e_ident[EI_MAG3] != 'F') {
        printf("file is not ELF type");
        munmap(map, fd.st_size);
        close(curr_file);
        curr_file = -1;
        return;
    }
    if (elf_file->e_ident[EI_DATA] == 1) {
        printf("data encoding scheme : 2's complement, little endian\n");
    } else if (elf_file->e_ident[EI_DATA] == 2) {
        printf("data encoding scheme : 2's complement, big endian\n");
    } else {
        printf("data encoding scheme : Invalid data encoding\n");
    }
    printf("entry point : %d\n", elf_file->e_entry);
    printf("section offset : %d\n", elf_file->e_shoff);
    printf("number of section header : %d\n", elf_file->e_shnum);
    printf("size of each section header entry : %d\n", elf_file->e_shentsize);
    printf("section offset of program header : %d\n", elf_file->e_phoff);
    printf("number of program header : %d\n", elf_file->e_phnum);
    printf("size of each program header entry : %d\n", elf_file->e_phentsize);
}

void print_section_names(){
    printf("%s", "not implemented yet\n");
}

void print_symbols(){
    printf("%s", "not implemented yet\n");
}


void quit(){
    printf(" -DONE- \n");
    exit(0);
}


int main(int argc, char **argv){

    struct fun_desc menu[] = {{"Toggle Debug Mode",toggle_debug_mode},{"Examine ELF File",examine_ELF_file},
            {"Print Section Names",print_section_names},{"Print Symbols",print_symbols},{"Quit",quit},{NULL,NULL}};

    int menuSize = sizeof(menu)/sizeof(menu[0]);

    int requested = 1;

    while (requested != 4 ){
        printf("%s\n","Please choose a function:");
        for (size_t i = 0; i < menuSize-1 ; ++i){
            printf("%d) %s\n", i, menu[i].name);
        }
        printf("%s","Option: ");

        char x = fgetc(stdin);
        requested = x-'0';
        x = fgetc(stdin);

        if (requested >= 0 && requested < menuSize-1){
            printf("%s\n","Within bounds");
            menu[requested].fun();

        }
        else {
            printf("%s\n","Not within bounds");
        }

    }







    return 0;

}