output: main.o vector.o
	gcc main.o vector.o -o output
	@echo "Compilação bem sucedida"

main.o: main.c
	gcc -c main.c

vector.o: vector.c vector.h
	gcc -c vector.c

clean:
	rm *.o output