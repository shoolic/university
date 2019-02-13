CC=gcc
CFLAGS=-m32 -g
LIB=`pkg-config --libs allegro-5  allegro_image-5`

ASM=nasm
AFLAGS=-f elf32 -g

all: prog

main.o: main.c
	$(CC) $(CFLAGS) -c main.c
func.o: func.nasm
	$(ASM) $(AFLAGS) func.nasm
prog: main.o func.o
	$(CC) $(CFLAGS) main.o func.o -o prog $(LIB) 
clean: 
	rm *.o
	rm prog