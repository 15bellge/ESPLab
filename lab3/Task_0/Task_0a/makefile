all: mergesort

mergesort: mergesort.o
	gcc -m32 -g -Wall -o mergesort mergesort.o

mergesort.o: lab3_mergesort.c
	gcc -m32 -g -Wall -c lab3_mergesort.c -o mergesort.o

.PHONY: clean

clean:
	rm -f *.o mergesort 
