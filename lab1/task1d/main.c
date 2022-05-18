#include <stdio.h>
#include <string.h>
#include <stdbool.h>
int main(int argc, char *argv[]){
	int value=0;
	bool secondargExist=true;
	bool plus = true;
	int c=0;
	int counter=0;
	bool flag=true;
	int counter2=0;
	int first=0;

	if(argc==1){
		secondargExist=false;
	}
	else{
		if(argv[1][2]<='9'){
			value=argv[1][2]-'0';
		}
		else{
			value=argv[1][2]-'A'+10;
		}
		if(!(value>=0 && value<16)){
			fprintf(stderr,"key is out of range\n");
			return 0;
		}
		if(strncmp(argv[1],"-e",2)==0){
			plus = false;
		}
	}

	FILE *fp = fopen(argv[2]+2,"r");
	if(fp==NULL){
		fprintf(stderr, "couldn't open file\n");
	}

	while((c=fgetc(fp))!=EOF){
		if(secondargExist==false){
			if(c>='A' && c<='Z'){
				fprintf(stderr,"%d %d\n", c,'.');
				printf("%c", '.');
				counter++;
			}
			else{
				fprintf(stderr,"%d %d\n", c,c);
				printf("%c",c);
			}
			if(c==10){
				printf("the number of letters: %d\n", counter);
				printf("argc = %d\n",argc);
				counter=0;
			}
		}
		else{
			if(plus==true){
				if(flag==true){
					first=c;
					flag=false;
				}
				if(c!=10){
					printf("%c",c);
				}
				else{
					for(int i=0;i<value;i++){
						printf("%c",first);
					}
					printf("\n");
					flag=true;
				}
			}
			else{
				if(c==10){
					printf("\n");
					counter2=0;
				}
				else{
					if(counter2<value){
						counter2++;
					}
					else{
						printf("%c",c);
					}
				}
			}
		}
	}
	return 0;
}
