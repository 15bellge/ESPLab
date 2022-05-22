all: shell looper

shell: pipeFunctions.o LineParser.o main.o
	gcc -m32 -g -Wall main.o  pipeFunctions.o LineParser.o -o shell

main.o: main.c
	gcc -m32 -g -Wall -c main.c -o main.o

pipeFunctions.o: pipeFunctions.c
	gcc -m32 -g -Wall -c -o pipeFunctions.o pipeFunctions.c

LineParser.o: LineParser.c
	gcc -m32 -g -Wall -c LineParser.c -o LineParser.o

looper: looper.o
	gcc -g -m32 -Wall looper.o -o looper

looper.o: looper.c
	gcc -m32 -g -Wall -c -o looper.o looper.c

.PHONY: clean

clean:
	rm -f *.o shell looper
