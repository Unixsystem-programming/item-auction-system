TARGETS = client server
CC = gcc
CFLAGS = -Wall -O3 -D_REENTRANT

LIBS= -lpthread

SRCS =$(wildcard *.c)
OBJS =$(SRCS:.c=.o)

all: ${TARGETS} clean

client : client.c
	$(cc) $(CFLAGS) -o $@ $? $(LIBS)

server : server.o
	$(cc) $(CFLAGS) -o $@ $? $(LIBS)

clean:
	rm -f core *.o a.out
