CC = gcc 
CFLAGS = -Wall -W -Wshadow -std=gnu99 
TARGETS = client server
 
all: $(TARGETS)

client : client.o

server : server.o

clean: 
	rm -f *.o	

distclean: clean
	rm -f $(TARGETS)
