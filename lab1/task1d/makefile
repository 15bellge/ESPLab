task1d: main.o
	gcc -m32 -g -Wall -o encoder main.o
main.o: main.c
	gcc -m32 -Wall -c main.c -o main.o
.PHONY: clean
clean:
	rm -f *.o task1d
