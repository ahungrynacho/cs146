CC = gcc
FLAGS = -std=c99 -ggdb
OBJECTS = nsh.o parse.o

pipe: pipe.o
	$(CC) $(FLAGS) pipe.o -o pipe

pipe.o: pipe.c
	$(CC) $(FLAGS) -c pipe.c

nsh: $(OBJECTS)
	$(CC) $(FLAGS) $(OBJECTS) -o nsh

nsh.o: nsh.c
	$(CC) $(FLAGS) -c nsh.c 

parse.o: parse.c parse.h
	$(CC) $(FLAGS) -c parse.c

clean:
	rm -f *.o core.* nsh
