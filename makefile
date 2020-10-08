CC = gcc
CFLAGS = -g

TARGETS = master palin

all: $(TARGETS)

master: master.o
	$(CC) -o $@ master.o $(LIBS)

palin: palin.o
	$(CC) -o $@ palin.o $(LIBS)

.PHONY: clean

.c .o:
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f core *.o $(TARGETS) *.out out.log