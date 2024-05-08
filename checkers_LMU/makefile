CC = gcc
CFLAGS = -Wall -Werror -Wpedantic 
all: sysprak-client

sysprak-client: sysprak-client.o performConnection.o config.o runningGame.o
	$(CC) $(CFLAGS)	sysprak-client.o performConnection.o config.o runningGame.o -o sysprak-client 

sysprak-client.o: sysprak-client.c
	$(CC) $(CFLAGS)	-c sysprak-client.c

performConnection.o: performConnection.c
	$(CC) $(CFLAGS)	-c performConnection.c

runningGame.o: runningGame.c
	$(CC) $(CFLAGS)	-c runningGame.c

config.o: config.c
	$(CC) $(CFLAGS)	-c config.c

clean:
	echo "Removing everything but the source files"
	rm sysprak-client.o performConnection.o config.o runningGame.o sysprak-client
