#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <fcntl.h>

//Implemente um programa que imprime a lista de argumentos recebidos na sua linha de comandos
// Com qualquer lista de argumentos.
// Mantendo o nome do ficheiro que corresponde o programa executável, experimente alterar
// o primeiro elemento da lista de argumentos (índice zero do argv).

int main(int argc, char** argv[])
{
    strcpy(argv[0], "exc");
    execv("Ex3", argv);
    return 0;
}
