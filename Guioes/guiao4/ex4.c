#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <fcntl.h>

//Escreva um programa redir que permita executar um comando, opcionalmente redirrecionando a entrada e/ou saída.
//O programa podeserá ser invocado, com:
//redir [-i fich_entrada] [-o fich_saida] comando arg1 arg2 ...


