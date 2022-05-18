all: task1a

task1a: task1a.o
	gcc -m32 -g -Wall -o task1a task1a.o

task1a.o: task1a.c
	gcc -m32 -g -Wall -c task1a.c -o task1a.o

.PHONY: clean

clean:
	rm -f *.o task1a
