#include <stdlib.h>
#include <stdio.h>
#include <string.h>
typedef struct virus{
	unsigned short sigSize;
	unsigned char* sig;
	char virusName[16];
}virus;

void printHex(unsigned char *buffer, unsigned short length, FILE *output){
	for(int i = 0; i<length; i++){
		fprintf(output, "%02X ",buffer[i]);
	}
}

void readVirus(virus* vir, FILE* input){
	if(fread(&vir->sigSize, 1, sizeof(unsigned short), input) < sizeof(unsigned short)){
		vir=NULL;
		return;
	}
	printf("size: %d\n",vir->sigSize);

	vir->sig = (unsigned char *)malloc(vir->sigSize*(sizeof(unsigned char)));
	if(fread(vir->sig, 1, vir->sigSize, input) < vir->sigSize){
		vir=NULL;
		free(vir->sig);
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

int main(int argc, char **argv){
	FILE *input = fopen("signatures","r");
	FILE *output = fopen("output.txt", "w");
	//fseek(input, 0 ,SEEK_SET);
	virus *vir = (virus *)malloc(sizeof(virus));
	while(!feof(input)){
		readVirus(vir, input);
		if(vir != NULL){
			printVirus(vir, output);
		}
		else{
			free(vir);
		}
	}
	fclose(output);
	fclose(input);
	return 0;
}
