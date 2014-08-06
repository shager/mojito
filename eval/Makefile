CC=gcc
CFLAGS=-std=gnu99

main: sender

sender: sender.c
	$(CC) $(CFLAGS) -o $@ $^
	
clean:
	rm -f sender
