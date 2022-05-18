#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct virus{
	unsigned short sigSize;
	unsigned char* sig;
	char virusName[16];
}virus;

typedef struct link link;

struct link{
	link *nextVirus;
	virus *vir;
};

void printHex(unsigned char *buffer, int length, FILE *output){
	for(int i = 0; i<length; i++){
		fprintf(output, "%02X ",buffer[i]);
	}
}

void readVirus(virus* vir, FILE* input){
	if(fread(&vir->sigSize, 1, sizeof(unsigned short), input) < sizeof(unsigned short)){
		vir=NULL;
		return;
	}

	vir->sig = (unsigned char *)malloc(vir->sigSize*(sizeof(unsigned char)));
	if(fread(vir->sig, 1, vir->sigSize, input) < vir->sigSize){
		free(vir->sig);
		vir=NULL;
		return;
	}	

	if(fread(vir->virusName, 1, 16, input) < 16){
		vir=NULL;
		return;
	}
}


void printVirus(virus* vir, FILE* output){
	fprintf(output, "Virus name: %s", vir->virusName);

	fprintf(output, "\nVirus size: %d", vir->sigSize);

	fprintf(output, "\nsignature: \n");
	printHex(vir->sig, vir->sigSize, output);
	fprintf(output,"\n\n");
}

void list_print(link *virus_list, FILE *output){
	link *temp = virus_list;
	while(temp != NULL){	
		if(temp->vir!=NULL && temp->vir->sig!=NULL) {
			printVirus(temp->vir, output);
		}
		temp = temp->nextVirus;
	}
}

link *list_append_as_first(link *virus_list, link *to_add){//add as first
	to_add->nextVirus = virus_list;
	return to_add;
}

link *list_append_at_end(link *virus_list, link *to_add){//add at end
	link *temp = virus_list;
	if(virus_list == NULL){
		return to_add;
	}
	while(temp->nextVirus != NULL){
		temp = temp->nextVirus;
	}
	temp->nextVirus = to_add;
	return virus_list;
}

void list_free(link *virus_list){
	free(virus_list);
}

link *loadSignatures(link *list, FILE *input){	
	while(!feof(input)){
		virus *next_virus = (virus *)malloc(sizeof(virus));
		readVirus(next_virus, input);
		if(next_virus->sig!=NULL){
			link *to_add = (link *)malloc(sizeof(input));
			to_add->vir = next_virus;
			to_add->nextVirus = NULL;		
			list = list_append_as_first(list, to_add);
		}
	}
	return list;
}

void printSignatures(link *list, FILE* output){
	if(list != NULL){
		list_print(list, output);
	}
}

void detect_virus(char *buffer, unsigned int size, link *virus_list){
	for(int i=0; i < size; i++){
		link *temp = virus_list;
		while(temp != NULL){
			bool flag = true;
			for(int j=0; j < temp->vir->sigSize; j++){
				if(memcmp(buffer+i+j, temp->vir->sig+j,1)!=0){
					flag = false;
					break;
				}
			}
			if(flag){
				printf("VIRUS WAS DETECTED!\n");
				printf("The starting byte location in the suspected file: %d\n", i);
				printf("Virus name: %s\n", temp->vir->virusName);
				printf("the size of the virus signature: %d\n\n", temp->vir->sigSize);
				break;
			}
			temp = temp->nextVirus;
		}
	}
}

void kill_virus(char *fileName, int signatureOffset, int signatureSize){
	FILE *file = fopen(fileName, "r+");
	char* to_write[signatureSize];
	memset(to_write, 0x90, signatureSize);
	fseek(file, signatureOffset, SEEK_SET);
	fwrite(to_write, 1, signatureSize,file);
	fclose(file);
}

int main(int argc, char **argv){
	FILE *input;
	FILE *output = fopen("output.txt", "w");
	link *virus_list = NULL;
	int option;
	char *virus_file_name = (char *)malloc((20)*sizeof(char));
	while(1){
		FILE *virus_file;
		printf("\nPlease choose a function:\n");
		printf("1) Load signatures\n");
		printf("2) Print signatures\n");
		printf("3) Detect viruses\n");
		printf("4) Fix file\n");
		printf("5) Quit\n");
		char num[20];
		fgets(num, 20, stdin);
		sscanf(num, "%d", &option);
		if(option == 1){
			printf("Please enter a signature file name:\n");
			char *name = (char *)malloc((20)*sizeof(char));
			char buffer[20];
			fgets(buffer, 20, stdin);
			sscanf(buffer, "%s", name);
			input = fopen(name, "r");
			virus_list = NULL;
			virus_list = loadSignatures(virus_list, input);
			free(name);
			fclose(input);
		}
		else if(option == 2){
			printSignatures(virus_list, output);
			fclose(output);			
		}
		else if (option == 3){
			printf("Please enter a file name:\n");			
			char name_buffer[20];
			fgets(name_buffer, 20, stdin);
			sscanf(name_buffer, "%s", virus_file_name);
			virus_file = fopen(virus_file_name, "r");
			char buffer[10000];
			fread(buffer, 1, sizeof(buffer), virus_file);
			int size = (sizeof(*buffer) < sizeof(*virus_file)) ? sizeof(buffer) : sizeof(virus_file);
			detect_virus(buffer, size, virus_list);
			fclose(virus_file);
		}
		else if(option==4){
			printf("Please enter the starting byte location:\n");
			int location;
			char location_buffer[3];
			fgets(location_buffer, 20, stdin);
			sscanf(location_buffer, "%d", &location);
			printf("Please enter the size of the virus sognature:\n");
			int size;
			char size_buffer[3];
			fgets(size_buffer, 20, stdin);
			sscanf(size_buffer, "%d", &size);
			kill_virus(virus_file_name, location, size);
		}
		else if (option == 5){
			exit(0);
		}
		else{
			printf("Not within bounds\n");
		}
	}
	free(virus_file_name);	
	list_free(virus_list);
	return 0;
}
