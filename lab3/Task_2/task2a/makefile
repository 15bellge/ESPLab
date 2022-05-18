all: task2a

task2a: task2a.o
	gcc -m32 -g -Wall -o task2a task2a.o

task2a.o: task2a.c
	gcc -m32 -g -Wall -c task2a.c -o task2a.o

.PHONY: clean

clean:
	rm -f *.o task2a
