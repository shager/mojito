# compiler
CC=gcc
# remove tool
RM=rm -f
#compiler flags
DEBUGFLAGS=-g
CFLAGS=-Wall -Werror -std=gnu99 $(DEBUGFLAGS)
#-Wpedantic -pedantic -pedantic-errors

# precompiled headers
GCH=bv_types.h.gch asm.h.gch
# object files
OBJ=test.o bv_types.o bv_list.o asm.o
# executables
EXECS=test

test: test.o bv_types.o asm.o
	$(CC) $(CFLAGS) -o $@ $^

test.o: test.c
	$(CC) $(CFLAGS) -c $^

bv_types.o: bv_types.c bv_types.h
	$(CC) $(CFLAGS) -c $^
	
asm.o: asm.c asm.h
	$(CC) $(CFLAGS) -c $^

clean:
	$(RM) $(OBJ) $(EXECS) $(GCH)
