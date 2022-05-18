all: task1

task1: main.o LineParser.o
	gcc -m32 -g -Wall main.o  LineParser.o -o task1

main.o: main.c
	gcc -m32 -g -Wall -c main.c -o main.o

LineParser.o: LineParser.c
	gcc -m32 -g -Wall -c LineParser.c -o LineParser.o

.PHONY: clean

clean:
	rm -f *.o task1
