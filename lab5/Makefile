CC = gcc
FLAGS = -std=c11 -ggdb
OBJECTS = main.o parse.o

main: $(OBJECTS)
	$(CC) $(FLAGS) $(OBJECTS) -o main

main.o: main.c
	$(CC) $(FLAGS) -c main.c

parse.o: parse.c parse.h
	$(CC) $(FLAGS) -c parse.c

clean:
	rm -f *.o main
