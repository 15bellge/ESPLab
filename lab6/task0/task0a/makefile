all: mypipe

mypipe: mypipe.o
	gcc -m32 -g -Wall mypipe.o -o mypipe

mypipe.o: mypipe.c
	gcc -m32 -g -Wall -c mypipe.c -o mypipe.o

.PHONY: clean

clean:
	rm -f *.o mypipe
