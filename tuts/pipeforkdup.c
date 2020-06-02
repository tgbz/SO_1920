//pipe dup fork and exec exemplo
//corre como: ./pipeforkdup.c date wc */
//Corre o equivalente no terminal a date | wc
#include <stdio.h>
#include <unistd.h>  
#include <sys/wait.h>

int main(int argc, char const *argv[])
{
    int pipefd[2], status, done = 0;
    pid_t cpid;

    pipe(pipefd);
    cpid=fork();
    if(cpid == 0){ 
        // filho esquerdo XX | YY
        close(pipefd[0]); // Fecha read end nao utilizado
        dup2(pipefd[1],STDOUT_FILENO);
        execlp(argv[1], argv[1], (char *) NULL);
    }
    cpid = fork();
    if (cpid == 0){
        //filho direito YY | XX
        close (pipefd[1]); //Fechar o unsed write end
        dup2(pipefd[0], STDIN_FILENO);
        execlp(argv[2], argv[2], (char *) NULL);
    }
    close(pipefd[0]); //Fecha os pipes para o EOF funcionar
    close(pipefd[1]); //Subil mas importante. O segundo filho não vai receber um EOF para lhe dar trigger para terminar enquando pelo menos um outro processo (o pai) tem o write end aberto

    //Parent reaps children exits
    waitpid(-1, &status, 0);
    waitpid(-1, &status, 0);
    return 0;
}
