CC = gcc
FLAGS = -std=c99 -ggdb
OBJECTS = nsh.o parse.o

nsh: $(OBJECTS)
	$(CC) $(FLAGS) $(OBJECTS) -o nsh

nsh.o: nsh.c
	$(CC) $(FLAGS) -c nsh.c 

parse.o: parse.c parse.h
	$(CC) $(FLAGS) -c parse.c

clean:
	rm -f *.o core.* nsh
