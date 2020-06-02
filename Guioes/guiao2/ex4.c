#include <stdio.h>
#include <unistd.h>  
#include <sys/wait.h>
//Implementar um programa que crie dez processos filhos que deverão executar em concorrência. O pai deverá esperar pelo fim da execução de todos os seus filhos, imprimindo os respectivos códigos de saída.
int main(int argc, char const *argv[])
{
    pid_t pid;
    int status;
    int numProcessos = 10;

    //criar os processos
    for (int i = 1; i<= numProcessos; i++){
        if((pid=fork())==0){
            printf("[Processo %d] pid = %d\n", i, getpid());
            _exit(i);
        }
    }

    for(int i = 1; i <= numProcessos; i++){
        pid_t terminated_pid = wait(&status);
        //se o filho retornou então WIFEXITED retorna TRUE
        //se o filho retornou -1 então WIFEXITED retorna TRUE e WEXITSTATUS retorna -1;
        if(WIFEXITED(status)){
            printf("[Pai] o processo %d saiu com código de saída %d\n", terminated_pid, WEXITSTATUS(status));
        }
        else{
            printf("[Pai] o processo %d saiu \n", terminated_pid);
        }
    }

    return 0;
}
