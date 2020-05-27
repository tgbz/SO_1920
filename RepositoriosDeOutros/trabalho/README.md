# SO
Trabalho Pratico da disciplina de Sistemas Operativos - Universidade do Minho 2016

#criar tudo
make all

#criar cliente
make sobucli

#criar servidor
make sobusrv

#criar livrarias - ficheiros *.c sem main(), trigger -c
gcc --ansi -pedantic -Wall -c ficheiro.c

#comandos importantes
	awk '{print $9}'
	ls -alr -d -1 "$PWD"/{*,.*}
	find * -type f
	find * -type d
