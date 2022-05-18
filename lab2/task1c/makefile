all: task1c

task1c: task1c.o
	gcc -m32 -g -Wall -o task1c task1c.o

task1c.o: task1c.c
	gcc -m32 -g -Wall -c task1c.c -o task1c.o

.PHONY: clean

clean:
	rm -f *.o task1c
