all:sobusrv sobucli

sobusrv:
	gcc -Wall --ansi -pedantic -g -o sobusrv server1.c enviaFicheiros.c exec.c

sobucli:
	gcc -Wall --ansi -pedantic -g -o sobucli cliente.c enviaFicheiros.c exec.c

clean:
	rm -f sobucli sobusrv *.o .a *~ Makefile.bak $(OUT)

