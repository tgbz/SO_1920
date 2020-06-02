#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <sys/wait.h>

//Escreva um programa que execute o comando wc num processo filho.
//O processo pai deve enviar ao filho através de um pipe anónimo uma sequência
//de linhas de texto introduzidas pelo utilizador no seu standard input.
//Recorra à técnica de redireccionamento estudada no guião anterior de modo
//A associar o standard input do processo filho ao descritor de leitura do pipe
//anónimo criado pelo pai
//Recorde a necessidade de fechar os descritores de escrita no pipe de modo a verificar-se
//a situação de end of file.

#define MAX_LINE_SIZE 1024

//Read line

ssize_t readln (int filedes, void* buffer, ssize_t numBytes){

    ssize_t res = 0;
    int i = 0;

    while(i < numBytes && (res = read(filedes, &buffer[i], 1))>0){
        if(((char*)buffer)[i]=='\n') return (i +1);
        i+= res;
    }
    return i;
}

//Main

int main(int argc, char const *argv[])
{
    int p[2];
    char buffer[MAX_LINE_SIZE];
    int res;
    int status;

    if (pipe(p)==-1){
        perror("Pipe não foi criado!");
        return -1;
    }

    switch(fork()){
        
        case -1:
        perror("Fork não foi efetuado!");
        return -1;


        case 0:
        //Código Filho
        close(p[1]);
        dup2(p[0],0);
        close(p[0]);
        execlp("wc","/bin/wc",NULL);
        _exit(0);


        default:
        //Código Pai
        close(p[0]);
        while((res = readln(0, buffer, MAX_LINE_SIZE))>0){
            write(p[1],buffer, res);
        }
        close(p[1]);
        wait(&status);
        if(WIFEXITED(status)){
            printf("[Pai] o filho terminou com %d\n", WEXITSTATUS(status));
        }
    }
    return 0;
}

