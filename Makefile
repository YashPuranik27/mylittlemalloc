CC = gcc
CFLAGS =-g -Wall -std=c99
LIBRARY = mymalloc.c
all: memgrind 

memgrind: memgrind.o $(LIBRARY)
        $(CC) $(CFLAGS) -o memgrind memgrind.o $(LIBRARY)

memgrind.o: memgrind.c mymalloc.h
        $(CC) $(CFLAGs) -c memgrind.c

clean:
        rm -f memgrind *.o
