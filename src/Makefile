C=gcc
RM=rm -f
#CFLAGS=-O3 -Wall -Werror -pedantic-errors -std=c11
CFLAGS=-O3 -std=c99

OBJ=test.o bv_types.o
EXECS=test

test: test.o bv_types.o
	$(CC) $(CFLAGS) -o test test.o bv_types.o

test.o: test.c
	$(CC) $(CFLAGS) -c $<

bv_types.o: bv_types.c bv_types.h
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) $(OBJ) $(EXECS)
