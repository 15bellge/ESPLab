all: hexaPrint

hexaPrint: hexaPrint.o
	gcc -m32 -g -Wall -o hexaPrint hexaPrint.o

hexaPrint.o: hexaPrint.c
	gcc -m32 -g -Wall -c hexaPrint.c -o hexaPrint.o

.PHONY: clean

clean:
	rm -f *.o hexaPrint 
