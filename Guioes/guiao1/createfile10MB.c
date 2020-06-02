#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <fcntl.h> /*O_RDONLY, O_WRONLY, O_CREAT, O_* */

#define BUFFER_SIZE 1024
#define FILE_SIZE 10*1024*1024 // 10Mb

//Função para criar um ficheiro
ssize_t createFile(){
    //Abrir Ficheiro
    int fd = open("./temp_file.txt", O_CREAT | O_WRONLY | O_TRUNC, 0600);
    ssize_t res = 0;

    //Popular Buffer
    char* buffer = malloc(sizeof(char)*BUFFER_SIZE);
    for (int i = 0; i < BUFFER_SIZE; i++){
        buffer[i] = 'a';
    }

    //Escrever para ficheiro
    for (int i = 0; i < FILE_SIZE/BUFFER_SIZE; i++){
        res += write(fd, buffer, BUFFER_SIZE);
    }

    //Fechar o descritor de ficheiros
    close(fd);
    free(buffer); //libertar memória
}

int main(int argc, char const *argv[])
{
    createFile();
    return 0;
}
