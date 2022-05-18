#include <stdio.h>
int main(){
	int iarray[] = {1,2,3};
	char carray[] = {'a','b','c'};
	int* iarrayPtr = &iarray[0];
	char* carrayPtr = &carray[0];
	for(int i=0;i<3;i++){
		printf("%d",*(iarrayPtr+i));
	}
	printf("\n");
	for(int i=0;i<3;i++){
		printf("%c",*(carrayPtr+i));
	}
	printf("\n");
	int* p;
	printf("%p\n", p);
}
