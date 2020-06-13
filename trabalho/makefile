CC = gcc

all: rem server argus

server:
	gcc server.c  -o server

argus:
	gcc client.c -o argus

rem:
	rm -rf server argus

clean:
	rm -rf log* server argus