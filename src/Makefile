# compiler
CC=gcc
# remove tool
RM=rm -f
#compiler flags
CFLAGS=-O3 -Wall -Werror -Wpedantic -pedantic -pedantic-errors -std=c99

# precompiled headers
GCH=bv_types.h.gch
# object files
OBJ=test.o bv_types.o bv_list.o
# executables
EXECS=test

test: test.o bv_types.o bv_list.o
	$(CC) $(CFLAGS) -o $@ $^

test.o: test.c
	$(CC) $(CFLAGS) -c $^

bv_types.o: bv_types.c bv_types.h
	$(CC) $(CFLAGS) -c $^

bv_list.o: bv_list.c bv_list.h
	$(CC) $(CFLAGS) -c $^

clean:
	$(RM) $(OBJ) $(EXECS) $(GCH)
