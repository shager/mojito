CC=gcc
CFLAGS=-std=gnu99 -Wall -Werror -Wpedantic -pedantic -pedantic-errors

main: sender

sender: sender.c
	$(CC) $(CFLAGS) -o $@ $^
	
clean:
	rm -f sender
