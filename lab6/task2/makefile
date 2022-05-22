all: shell looper

shell: main.o LineParser.o
	gcc -m32 -g -Wall main.o  LineParser.o -o shell

main.o: main.c
	gcc -m32 -g -Wall -c main.c -o main.o

LineParser.o: LineParser.c
	gcc -m32 -g -Wall -c LineParser.c -o LineParser.o

looper: looper.o
	gcc -g -m32 -Wall looper.o -o looper

looper.o: looper.c
		gcc -m32 -g -Wall -c -o looper.o looper.c


.PHONY: clean

clean:
	rm -f *.o shell looper
