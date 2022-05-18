#include <stdio.h>
#include <stdlib.h>

void printHex(unsigned char *buffer, int length){
	for(int i = 0; i<length; i++){
		printf("%02x ",buffer[i]);
	}
}

int main(int argc, char **argv){
	unsigned char buffer[1];
	FILE *fp = fopen(*(argv+1), "r"); 
	while(fread(buffer, sizeof *buffer, 1, fp) == 1 ){
		printHex(buffer,1);
	}
	printf("\n");
	fclose(fp);
}
