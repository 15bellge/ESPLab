all: mypipeline

mypipeline: LineParser.o mypipeline.o
	gcc -m32 -g -Wall LineParser.o mypipeline.o -o mypipeline

mypipeline.o: mypipeline.c
	gcc -m32 -g -Wall -c mypipeline.c -o mypipeline.o

LineParser.o: LineParser.c
	gcc -m32 -g -Wall -c LineParser.c -o LineParser.o

.PHONY: clean

clean:
	rm -f *.o mypipeline
