#include <stdio.h>

int main(int argc, char *argv[]){
	char c=0;
	while((c=fgetc(stdin))!=EOF){
		printf("%c",c);
	}
	printf("\n");
	return 0;
}
