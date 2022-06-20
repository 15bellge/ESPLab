all: main

main: skeleton.o
	ld -m elf_i386 skeleton.o -o main.exe

skeleton.o: skeleton.s
	nasm -f elf32 skeleton.s

.PHONY: clean

clean:
	rm -f *.o main