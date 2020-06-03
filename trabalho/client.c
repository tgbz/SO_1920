#include <stdio.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> 


#define MAX_LINE_SIZE 1024
#define BUFFER_SIZE 1024


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


void interpretaArgs(int qt, const char  *args[]) {

    int fd = open("fifo",O_WRONLY);

    if (strcmp(args[0],"ajuda")==0) {
        ajuda();
    }
    /*else ifs para comando que recebem resposta
    TODO: Pensar em como vai receber as respostas.. Outro pipe?
    */
    else {

        write(fd,"\"", 1);
        for (int i = 0; i < qt; i++) {
            write(fd, args[i], strlen(args[i]));
            if(i < qt-1)
              write(fd,"\"", 1);
        }
        write(fd, "\"\n", 2);
        
    }
    close(fd);
}


int main(int argc, char const *argv[])
{
    int i;
    char readbuf[BUFFER_SIZE];


    if (argc==1) {
        /*shell*/
        ajuda();

        while(1) {
        write(1, "argus$ ", 7);
        fgets(readbuf, BUFFER_SIZE, stdin);
        

        const char *argv[2];
        argv[0] = strtok(readbuf, "\"\n"); //FIXME - plicas vs aspas vs acentos latex é uma merda
        argv[1] = strtok(NULL, "\"\n");

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
    

