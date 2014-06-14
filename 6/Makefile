CC=gcc
#STANDARD=-ansi -pedantic-errors
DEBUG_OPTION=-ggdb
CFLAGS=-Wall $(STANDARD) $(DEBUG_OPTION)
LDFLAGS=-Wall $(STANDARD) $(DEBUG_OPTION)

all: 6-1 6-2

6-1: 6-1.o
	$(CC) $< -o $@ $(LDFLAGS)

6-2: 6-2.o
	$(CC) $< -o $@ $(LDFLAGS)

6-1.o: 6-1.c
	$(CC) -c $< -o $@ $(CFLAGS)

6-2.o: 6-2.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm *.o

.PHONY: clean all
