CC=gcc
CFLAGS=-std=gnu99 -O0 -ggdb

default: asm

asm: asm.c asm.h
	$(CC) $(CFLAGS) -o $@ $^

clean:
	@rm -f asm.s
	@rm -f asm
