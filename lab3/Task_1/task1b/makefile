all: task1b

task1b: task1b.o
	gcc -m32 -g -Wall -o task1b task1b.o

task1b.o: task1b.c
	gcc -m32 -g -Wall -c task1b.c -o task1b.o

.PHONY: clean

clean:
	rm -f *.o task1b
