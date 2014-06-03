C=gcc
RM=rm -f
CFLAGS=-O3 -Wall -Werror -pedantic-errors -std=c11
#CFLAGS=-O3 -std=c11

OBJ=test.o
EXECS=test

test: test.o
	$(CC) $(CFLAGS) -o test $<

test.o: test.c
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) $(OBJ) $(EXECS)
