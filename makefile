CC=gcc
CFLAGS=-w

all: cot

cot: cot_MAIN.c functions.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f cot
