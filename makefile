CC = gcc
CFLAGS = -Wall -g

TARGETS = master palin

all: $(TARGETS)

master: master.c
	$(CC) $(CFLAGS) -o master master.c

palin: palin.c
	$(CC) $(CFLAGS) -o palin palin.c

.PHONY: clean

clean:
	rm $(TARGETS) *.o