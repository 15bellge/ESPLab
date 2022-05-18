#include <stdio.h>
#include <stdlib.h>
#include <string.h>
char censor(char c) {
	if(c == '!')
		return '*';
	else
		return c;
}

char* map(char *array, int array_length, char (*f) (char)){
	char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
	for(int i=0;i<array_length;i++){
		mapped_array[i] = f(array[i]);
	}
	return mapped_array;
}

/* Gets a char c and returns its encrypted form by adding 2 to its value.
If c is not between 0x41 and 0x7a it is returned unchanged */
char encrypt(char c){
	int encrypted_c = c+2;
	return c>='A' && c<='z' ? encrypted_c : c;
}

 /* Gets a char c and returns its decrypted form by reducing 2 to its value
If c is not between 0x41 and 0x7a it is returned unchanged */
char decrypt(char c){
	int decrypted_c = c-2;
	return c>='A' && c<='z' ? decrypted_c : c;
}
/* dprt prints the value of c in a decimal representation followed by a
new line, and returns c unchanged. */
char dprt(char c){
	printf("%d\n",c);
	return c;
}
/* If c is a number between 0x41 and 0x7a, cprt prints the character of ASCII value c followed by a new line. Otherwise, cprt prints the dot ('*') character. After printing, cprt returns the value of c unchanged. */
char cprt(char c){
	printf(c>='A' && c<='z' ? "%c\n" : "*\n", c);
	return c;
}
/* Ignores c, reads and returns a character from stdin using fgetc. */
char my_get(char c){
	return fgetc(stdin);
}

/* Gets a char c, and if the char is 'q' ,ends the program with exit code 0.Otherwise returns c. */
char quit(char c){
	if(c == 'q')
		exit(0);
	return c;
}

struct fun_desc{
	char *name;
	char (*fun)(char);
};

int main(int argc, char **argv){
	char* carray = malloc(5);
	carray[0]='\0';
	struct fun_desc menu[] = {{"Censor", censor},{"Encrypt", encrypt},{"Decrypt", decrypt},{"Print dec", dprt},{"Print string", cprt},{"Get string", my_get},{"Quit", quit},{"NULL", NULL}};
	int option;
	while(option != 6){
		printf("\nPlease choose a function:\n");
		for(int i=0;i<7;i++){
			printf("%d) %s\n", i, menu[i].name);
		}
		printf("Option: ");
		char num[20];
		fgets(num, 20, stdin);
		sscanf(num, "%d", &option);
		if(option >= 0 && option <= 6){
			printf("Within bounds\n");
			char (*func_ptr)(char);
			func_ptr = menu[option].fun;
			carray = map(carray, 5, func_ptr);
		}
		else{
			printf("Not within bounds\n");
			exit(0);
		}
	}
	return 1;
}
