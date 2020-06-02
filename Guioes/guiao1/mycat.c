#include<stdio.h>
#include<unistd.h> /*for STDOUT_FILENO*/
#include<stdlib.h>
#include<fcntl.h>

int main(int argc, char const *argv[])
{
    int fd, i, ch;
    for(i=1; i<argc; i++){ //Iterar o loop para a contagem de argumento total
        fd = open(argv[1], O_RDONLY); //Abrir o ficheiro em READONLY mode
        if(fd<0){
            perror("open");
            goto OUT; //Fase de Return 
        }
        while (read(fd,&ch,1)){ //Ler um byte de cada vez 
            write(STDOUT_FILENO,&ch,1); //Escrever um byte no Stdout
        }
        close(fd); //Fecha o file descriptor
    }
    return 0;
    OUT:
    return -1;
}
