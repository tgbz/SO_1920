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

//Funçãod e leitura de um pipe
void readFromPipe(char pipeName[]) {
    int fd;
    int lineChars;
    char buffer[BUFFER_SIZE];
    //Abre o pipe para leitura
    if ((fd = open(pipeName, O_RDONLY)) == -1)
    {
        perror("Erro ao abrir pipe com nome");
        return;
    }
    else
    {
        printf("Output:\n");
        while (1)
        {
            //le do fd (Resposta do servidor)
            lineChars = read(fd, buffer, BUFFER_SIZE*2);
            buffer[lineChars] = '\0';
            if(lineChars==0){
                break;
            }
            //Escreve no stdout
            write(1,buffer,strlen(buffer));
        }
    }
    close(fd);
    return;
}

//Limpa um buffer
void clearBuf(char* b){
    int i;
    for (i = 0; i < BUFFER_SIZE; i++)
        b[i] = '\0';
}

//Imprime o menu de ajuda
int ajuda(int method) {
    if (method==0) {
    write(1,"\ntempo-inactividade segs\n",25);
    write(1,"tempo-execucao segs\n",20);
    write(1,"executar \"comando1 args | comando2 | comando3\"\n",47);
    write(1,"listar\n",7);
    write(1,"terminar nTar\n",14);
    write(1,"historico\n",10);
    write(1,"output tarefa\n",14);
    write(1,"ajuda\n",6);
    write(1,"exit\n\n",6);
    }
    else if (method==1){
    write(1,"\ntempo-inactividade: -i segs\n",29);
    write(1,"tempo-execucao: -m segs\n",24);
    write(1,"executar: -e \"comando1 args | comando2 | comando3\"\n",51);
    write(1,"listar: -l\n",11);
    write(1,"terminar: -t nTar\n",18);
    write(1,"historico: -r\n",14);
    write(1,"output: -o tarefa\n",18);
    write(1,"ajuda: -h\n\n",11);
    }
}

//Verifica se a string lida é um numero
int notNumber(const char *number) {
    for (int i=0;i<strlen (number); i++)
        if (!isdigit(number[i]))
            return 1;
    return 0;
}

//Envia mensagem ao servidor
void sendMessage(int qt, const char * args[], int pipeNumber) {
    //Abre o ficheiro de escrita para o servidor
    int fd = open("/tmp/fifo",O_WRONLY);
    char str[MAX_LINE_SIZE];
    //Concatena os argumentos da funcao a executar
    for (int i = 0; i < qt; i++) {
        char strAux[256];
        sprintf(strAux, "%s#", args[i]);
        strcat(str,strAux);
    }
    //Passa o pid para string e concatena
    if(pipeNumber!=-1) {
        char pipe[16];
        sprintf(pipe,"%d#",pipeNumber);
        strcat(str,pipe);
    }
    strcat(str,"\n");
    //Escreve no fifo para o servidor
    write(fd,str,strlen(str));
    close(fd);
}

//Função de parse para enviar pedidos ao servidor
void interpretaArgs(int qt, const char  *args[], int method) {
    pid_t pid = getpid();
    char pipeName[1024];
    sprintf(pipeName,"/tmp/%d",pid);

    //Tempo de inatividade
    if ((method==0 && strcmp(args[0],"tempo-inactividade")==0)  || (method==1 && strcmp(args[0],"-i")==0)) {
        if (qt!=2 || notNumber(args[1])) {
            write(1,"\nAlgo está errado\nAJUDA:",24);
            ajuda(method);
        } else{
            args[0] = "tempo-inactividade";
            sendMessage(qt,args, -1);}
    }
    //Tempo de execucao
    else if ((method==0 && strcmp(args[0],"tempo-execucao")==0) || (method==1 && strcmp(args[0],"-m")==0)) {
        if (qt!=2 || notNumber(args[1])) {
            write(1,"\nAlgo está errado\nAJUDA:",24);
            ajuda(method);
        } else{
            args[0] = "tempo-execucao";
            sendMessage(qt,args, -1);}
    } 
    //Executar
    else if ((method==0 && strcmp(args[0],"executar")==0) || (method==1 && strcmp(args[0],"-e")==0)) {
        if (qt!=2){
            write(1,"\nAlgo está errado\nAJUDA:",24);
            ajuda(method);
        } else {
            args[0] = "executar";
            //Criar o pipe com nome para leitura da resposta do servidor
            mkfifo(pipeName, 0666);
            sendMessage(qt,args,pid);
            //Le a resposta do servidor
            readFromPipe(pipeName);
        }
    } 
    //Listar
    else if ((method==0 && strcmp(args[0],"listar")==0)  || (method==1 && strcmp(args[0],"-l")==0)) {
        if (qt!=1){
            write(1,"\nAlgo está errado\nAJUDA:",24);
            ajuda(method);
        } else{
            args[0] = "listar";
            //Criar o pipe com nome para leitura da resposta do servidor
            mkfifo(pipeName, 0666);
            sendMessage(qt,args,pid);
            //Le a resposta do servidor
            readFromPipe(pipeName);
        }
    } 
    //Terminar
    else if ((method==0 && strcmp(args[0],"terminar")==0) || (method==1 && strcmp(args[0],"-t")==0)) {
        if (qt!=2 || notNumber(args[1])) {
            write(1,"\nAlgo está errado\nAJUDA:",24);
            ajuda(method);
        } else{
            args[0] = "terminar";
            sendMessage(qt,args,-1);}
    } 
    //Historico
    else if ((method==0 && strcmp(args[0],"historico")==0) || (method==1 && strcmp(args[0],"-r")==0)) {
        if (qt!=1){
            write(1,"\nAlgo está errado\nAJUDA:",24);
            ajuda(method);
        } else{
            args[0] = "historico";
            //Criar o pipe com nome para leitura da resposta do servidor
            mkfifo(pipeName, 0666);
            sendMessage(qt,args,pid);
            //Le a resposta do servidor
            readFromPipe(pipeName);
        }
    } 
    //Ajuda
    else if (strcmp(args[0],"ajuda")==0 || strcmp(args[0],"-h")==0) {
        if (qt!=1){
            write(1,"\nAlgo está errado\nAJUDA:",24);
            ajuda(method);
        } else
            ajuda(method);
    } 
    //Output
    else if ((method==0 && strcmp(args[0],"output")==0) || (method==1 && strcmp(args[0],"-o")==0)) {
        if (qt!=2 || notNumber(args[1])) {
            write(1,"\nAlgo está errado\nAJUDA:",24);
            ajuda(method);
        } else{
            args[0] = "output";
            //Criar o pipe com nome para leitura da resposta do servidor
            mkfifo(pipeName, 0666);
            sendMessage(qt,args,pid);
            //Le a resposta do servidor
            readFromPipe(pipeName);
        }
    } 
    //Exit
    else if (method==0 && strcmp(args[0],"exit")==0){
        _exit(0);
    }
    //Comando inválido
    else {
        write(1,"\nComando Inválido\nAJUDA:",24);
        ajuda(method);
    }
}


int main(int argc, char const *argv[])
{
    int i;
    char readbuf[BUFFER_SIZE];
    clearBuf(readbuf);


    if (argc==1) {
        //Shell
        ajuda(0);

        while(1) {
        write(1, "argus$ ", 7);
        fgets(readbuf, BUFFER_SIZE, stdin);
        

        const char *argv[2];
        argv[0] = strtok(readbuf, " \"\n");
        argv[1] = strtok(NULL, "\"\n");

        int argc = (argv[1]==NULL) ? 1 : 2;

        //Interpreta o comando
        interpretaArgs(argc, argv, 0);

        clearBuf(readbuf);
        }
    }
    //Linha de comandos
    else {
        for(i = 0; i < argc - 1; i++)
            argv[i] = argv[i + 1];
        //Interpreta o comando
        interpretaArgs(argc-1, argv, 1);
    }


    return 0;
    }
    

