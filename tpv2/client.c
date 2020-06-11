#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> 
#include <ctype.h>
#include <time.h> 
#include <unistd.h>


#define MAX_LINE_SIZE 1024
#define BUFFER_SIZE 1024

void readFromPipe(char pipeName[]) {
    int fd;
    int lineChars;
    char buffer[BUFFER_SIZE];
    if ((fd = open(pipeName, O_RDONLY)) == -1)
    {
        perror("Erro ao abrir pipe com nome");
        return;
    }
    else
    {
        printf("Pipe com nome aberto para leitura!\nOutput:\n");
        while (1)
        {
            lineChars = read(fd, buffer, BUFFER_SIZE*2);
            buffer[lineChars] = '\0';
            if(lineChars==0){
                break;
            }

            printf("%s",buffer);
        }
    }
    close(fd);
    return;
}

void clearBuf(char* b){
    int i;
    for (i = 0; i < BUFFER_SIZE; i++)
        b[i] = '\0';
}


int ajuda() {
    printf("\ntempo-inactividade segs\n");
    printf("tempo-execucao segs\n");
    printf("executar ’comando1 args | comando2 | comando3’\n");
    printf("listar\n");
    printf("terminar 1\n");
    printf("historico\n");
    printf("ajuda\n");
    printf("output tarefa\n\n");
}

int notNumber(const char *number) {
    for (int i=0;i<strlen (number); i++)
        if (!isdigit(number[i]))
            return 1;
    return 0;
}

void sendMessage(int qt, const char * args[], int pipeNumber) {
    int fd = open("/tmp/fifo",O_WRONLY);
    char str[MAX_LINE_SIZE];
    for (int i = 0; i < qt; i++) {
        char strAux[256];
        sprintf(strAux, "%s#", args[i]);
        strcat(str,strAux);
    }
    if(pipeNumber!=-1) {
        char pipe[16];
        sprintf(pipe,"%d#",pipeNumber); //FIXME SPRINTF
        strcat(str,pipe);
    }
    strcat(str,"\n");
    write(fd,str,strlen(str));
    close(fd);
}

void interpretaArgs(int qt, const char  *args[]) {
    pid_t pid = getpid();
    char pipeName[1024];
    sprintf(pipeName,"/tmp/%d",pid);

    if (strcmp(args[0],"tempo-inactividade")==0) {
        if (qt!=2 || notNumber(args[1])) {
            printf("\nAlgo está errado\nAJUDA:");
            ajuda();
        } else
            sendMessage(qt,args, -1);
    } else if (strcmp(args[0],"tempo-execucao")==0) {
        if (qt!=2 || notNumber(args[1])) {
            printf("\nAlgo está errado\nAJUDA:");
            ajuda();
        } else
            sendMessage(qt,args, -1);
    } else if (strcmp(args[0],"executar")==0) {
        if (qt!=2){
            printf("\nAlgo está errado\nAJUDA:");
            ajuda();
        } else {
            mkfifo(pipeName, 0666);
            sendMessage(qt,args,pid);
            readFromPipe(pipeName);
            //HAS RESPONSE
        }
    } else if (strcmp(args[0],"listar")==0) {
        if (qt!=1){
            printf("\nAlgo está errado\nAJUDA:");
            ajuda();
        } else{
            mkfifo(pipeName, 0666);
            sendMessage(qt,args,pid);
            readFromPipe(pipeName);
            //HAS RESPONSE
        }
    } else if (strcmp(args[0],"terminar")==0) {
        if (qt!=2 || notNumber(args[1])) {
            printf("\nAlgo está errado\nAJUDA:");
            ajuda();
        } else
            sendMessage(qt,args,-1);
    } else if (strcmp(args[0],"historico")==0) {
        if (qt!=1){
            printf("\nAlgo está errado\nAJUDA:");
            ajuda();
        } else{
            mkfifo(pipeName, 0666);
            sendMessage(qt,args,pid);
            readFromPipe(pipeName);
            //HAS RESPONSE
        }
    } else if (strcmp(args[0],"ajuda")==0) {
        if (qt!=1){
            printf("\nAlgo está errado\nAJUDA:");
            ajuda();
        } else
            sendMessage(qt,args,-1);
    } else if (strcmp(args[0],"output")==0) {
        if (qt!=2 || notNumber(args[1])) {
            printf("\nAlgo está errado\nAJUDA:");
            ajuda();
        } else{
            sendMessage(qt,args,-1);
            //HAS RESPONSE
        }
    } else {
        printf("\nComando Inválido\nAJUDA:");
        ajuda();
    }
}


int main(int argc, char const *argv[])
{
    int i;
    char readbuf[BUFFER_SIZE];
    clearBuf(readbuf);


    if (argc==1) {
        /*shell*/
        ajuda();

        while(1) {
        write(1, "argus$ ", 7);
        fgets(readbuf, BUFFER_SIZE, stdin);
        

        const char *argv[2];
        argv[0] = strtok(readbuf, " '\n"); //FIXME - plicas vs aspas vs acentos latex é uma merda
        argv[1] = strtok(NULL, "'\n");

        int argc = (argv[1]==NULL) ? 1 : 2;


        interpretaArgs(argc, argv);

        clearBuf(readbuf);
        }
    }
    else {
        for(i = 0; i < argc - 1; i++)
            argv[i] = argv[i + 1];
        interpretaArgs(argc-1, argv);
    }


    return 0;
    }
    

