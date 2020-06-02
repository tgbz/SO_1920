#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <fcntl.h> /*O_RDONLY, O_WRONLY, O_CREAT, O_* */
#include <stdio.h>

#define BUFFER_SIZE 1024

int main(int argc, char const *argv[]){
    int srcFD, destFD, nbread, nbwrite;
    char *buff[BUFFER_SIZE];
    //Checa se ambos os ficheiros de destino e fonte são recebidos 
    if (argc != 3 || argv[1] == "--help"){
        printf("\nUsage: cpcmd source_file destiantion_file \n");
        exit(EXIT_FAILURE);
    }
    //Abrir o Ficheiro Source
    srcFD = open(argv[1],O_RDONLY);

    if(srcFD==-1){
        printf("\nError opening file %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    /*Abir ficheiro destino com as respetivas flags e modos O_CREAT e O_TRUNC 
    para truncar ficheiros existentes ou criar um novo ficheiro.
    S_IXXXX são permissões para grupos de utilizadores e outros*/
    destFD=open(argv[2],O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if(destFD == -1){
        printf("\nError opening file %s\n", argv[2]);
        exit(EXIT_FAILURE);
    }
    /*Começa a transferir a informação a partir do ficheiro src para o ficheiro até chegar a EOF*/
    while ((nbread = read(srcFD, buff, BUFFER_SIZE))>0){
        if (write(destFD,buff,nbread) != nbread)
        printf("\nError in writint data to %s\n", argv[2]);
    }
    if(nbread== -1){
        printf("\nError in reading data from %s\n",argv[1]);
    }
    if(close(srcFD)==-1){
        printf("\nError in closing file %s\n",argv[1]);
    }
    if(close(destFD)==-1){
        printf("\nError in closing file %s\n",argv[2]);
    }
    exit(EXIT_SUCCESS);
    return 0;
}

