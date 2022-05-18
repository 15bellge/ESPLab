
all: myecho

myecho: myecho.o
	gcc -m32 -g -Wall -o myecho myecho.o

myecho.o: myecho.c
	gcc -m32 -Wall -c myecho.c -o myecho.o

.PHONY: clean

clean: 
	rm -f *.o myecho


