CC = gcc
CFLAGS = -Wall -g


EXECUTE : Programa.o Main.c
	$(CC) $(CFLAGS) Programa.o Main.c -o EXECUTE

Programa.o: Programa.c 
	$(CC) $(CFLAGS) -c Programa.c -o Programa.o


clean:
	rm EXECUTE
	rm *.o