CC = gcc

all: rem server argus

server:
	gcc server.c  -o server -w

argus:
	gcc client.c -o argus -w

rem:
	rm -rf server argus

clean:
	rm -rf log* server argus