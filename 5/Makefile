#
# 实验五的Makefile
#
#
#           叶剑飞
#           2014年6月18日
#

CC=gcc
#STANDARD=-ansi -pedantic-errors
DEBUG_OPTION=-ggdb
CFLAGS=-Wall $(STANDARD) $(DEBUG_OPTION)
LDFLAGS=-Wall $(STANDARD) $(DEBUG_OPTION)

all: 5-3 5-4

5-3: 5-3.o
	$(CC) $< -o $@ $(LDFLAGS)

5-4: 5-4.o
	$(CC) $< -o $@ $(LDFLAGS)

5-3.o: 5-3.c
	$(CC) -c $< -o $@ $(CFLAGS)

5-4.o: 5-4.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm *.o

.PHONY: clean all
