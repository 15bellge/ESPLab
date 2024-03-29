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

void print_section_names() {
    if (elf_file == NULL) {
        printf("file does not exist");
    } else {
        Elf32_Shdr *string_row = map + elf_file->e_shoff + (elf_file->e_shstrndx * elf_file->e_shentsize);
        for (int i = 0; i < elf_file->e_shnum; i++) {
            Elf32_Shdr *row = map + elf_file->e_shoff + (i * elf_file->e_shentsize);
            char *name = map + string_row->sh_offset + row->sh_name;
            if (debug_mode) {
                printf("[%d] %-12s\t\t %-12x\t %-12x\t %-12x\t %-12x\t %-12x\t %-12x\t\n", i, name, row->sh_addr,
                       row->sh_offset, row->sh_size,
                       row->sh_type,
                       elf_file->e_shstrndx, elf_file->e_shoff + (i * elf_file->e_shentsize));
            } else {
                printf("[%d] %-12s\t\t %-12x\t %-12x\t %-12x\t %-12x\t\n", i, name, row->sh_addr, row->sh_offset,
                       row->sh_size,
                       row->sh_type);
            }
        }
    }
}

Elf32_Shdr *get_table(char *name_of_table) {
    if (elf_file == NULL) {
        printf("file does not exist");
    } else {
        //Elf32_Shdr *sections_table = map + elf_file->e_shoff;
        Elf32_Shdr *string_row = map + elf_file->e_shoff + (elf_file->e_shstrndx * elf_file->e_shentsize);
        for (int i = 0; i < elf_file->e_shnum; i++) {
            Elf32_Shdr *row = map + elf_file->e_shoff + (i * elf_file->e_shentsize);
            char *name = map + string_row->sh_offset + row->sh_name;
            if (strcmp(name, name_of_table) == 0)
                return row;
        }
    }
    return NULL;
}

void print_symbol_table(Elf32_Shdr *table_name) {
    Elf32_Shdr *strtab = get_table(".strtab");
    Elf32_Shdr *shstrtab = get_table(".shstrtab");
    if (table_name != NULL) {
        int number_of_entries = table_name->sh_size / sizeof(Elf32_Sym);
        if (debug_mode) {
            printf("number of entries: %d\n", number_of_entries);
        }
        for (int i = 0; i < number_of_entries; i++) {
            Elf32_Sym *row = map + table_name->sh_offset + (i * sizeof(Elf32_Sym));
            char *section_name;
            if (row->st_shndx == 0xFFF1) {
                section_name = "ABS";
            } else if (row->st_shndx == 0x0) {
                section_name = "UNK";
            } else {
                Elf32_Shdr *section_entry = map + elf_file->e_shoff + (row->st_shndx * elf_file->e_shentsize);
                section_name = map + shstrtab->sh_offset + section_entry->sh_name;
            }
            char *symbol_name = map + strtab->sh_offset + row->st_name;
            if (debug_mode) {
                printf("[%d] %-12x\t %-12d\t %-12s\t\t %-12s\t\t %-12d\t %-12d\n", i, row->st_value, row->st_shndx, section_name,
                       symbol_name, row->st_name, row->st_shndx);
            } else {
                printf("[%d] %-12x\t %-12d\t %-12s\t\t %-12s\n", i, row->st_value, row->st_shndx, section_name,
                       symbol_name);
            }
        }
    }
}

void print_symbols() {
    if (elf_file == NULL) {
        printf("file does not exist");
    } else {
        Elf32_Shdr *dynsym_table_entry_point = get_table(".dynsym");
        Elf32_Shdr *symtab_table_entry_point = get_table(".symtab");
        print_symbol_table(dynsym_table_entry_point);
        print_symbol_table(symtab_table_entry_point);
    }
}

void quit() {
    printf(" -DONE- \n");
    exit(0);
}

int main(int argc, char **argv) {
    struct fun_desc menu[] = {{"Toggle Debug Mode",   toggle_debug_mode},
                              {"Examine ELF File",    examine_ELF_file},
                              {"Print Section Names", print_section_names},
                              {"Print Symbols",       print_symbols},
                              {"Quit",                quit},
                              {NULL,                  NULL}};
    int menuSize = sizeof(menu) / sizeof(menu[0]);
    int requested = 1;
    while (requested != 4) {
        printf("%s\n", "Please choose a function:");
        for (size_t i = 0; i < menuSize - 1; ++i) {
            printf("%d) %s\n", i, menu[i].name);
        }
        printf("%s", "Option: ");
        char x = fgetc(stdin);
        requested = x - '0';
        x = fgetc(stdin);
        if (requested >= 0 && requested < menuSize - 1) {
            printf("%s\n", "Within bounds");
            menu[requested].fun();
        } else {
            printf("%s\n", "Not within bounds");
        }
    }
    return 0;
}