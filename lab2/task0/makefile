all: count-words

count-words: count-words.o
	gcc -m32 -g -Wall -o count-words count-words.o

count-words.o: count-words.c
	gcc -m32 -g -Wall -c count-words.c -o count-words.o

.PHONY: clean

clean:
	rm -f *.o count-words 
