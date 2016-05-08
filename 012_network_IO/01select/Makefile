CPPFLAGS=-I./include
CFLAGS=-g -Wall
LDFLAGS=-lpthread

src=$(wildcard *.c)
object=$(patsubst %.c,%.o,$(src))
CC=gcc

target=app

all:server client
	
#target:$(object)
#	$(CC)  $^ -o $(target)

server:server.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS) -o $@

client:client.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS) -o $@

%.c:%.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $^ -o $@

clean:
	-@rm -rf server client *.o
