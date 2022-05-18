all: addresses

addresses: addresses.o
	gcc -m32 -g -Wall -o addresses addresses.o

addresses.o: addresses.c
	gcc -m32 -g -Wall -c addresses.c -o addresses.o

.PHONY: clean

clean:
	rm -f *.o addresses
