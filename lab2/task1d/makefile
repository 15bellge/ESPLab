all: task1d

task1d: task1d.o
	gcc -m32 -g -Wall -o task1d task1d.o

task1d.o: task1d.c
	gcc -m32 -g -Wall -c task1d.c -o task1d.o

.PHONY: clean

clean:
	rm -f *.o task1d
