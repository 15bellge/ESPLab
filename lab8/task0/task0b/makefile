#All Targets

all: myELF

myELF: main.o
		gcc -g -m32 -Wall -o myELF main.o

main.o: main.c
		gcc -m32 -g -Wall -c -o main.o main.c

.PHONY: clean

clean:
		rm -f *.o myELF