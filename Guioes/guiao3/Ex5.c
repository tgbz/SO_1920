#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <fcntl.h>

//Implemente um programa que execute concorrentemente
//Uma lista de executáveis especificados como argumentos da linha de comando.
//Considere os executáveis sem quaisquer argumentos próprios.
//O Programa deverá esperar pelo fim da execução de todos processos por si criados.

int main(int argc, char const *argv[])
{
    pid_t pid;
    int status;
    int exec_ret;

    for (int i = 1; i<argc ; i++){
        if((pid=fork())==0){
            printf("[Processo %d] vou executar o comando %s\n", getpid(), argv[i]);
            exec_ret = execlp(argv[i],argv[i],NULL);
            _exit(exec_ret);
        }
    }
    
    for(int i = 1; i < argc; i++){
        pid_t terminated_pid = wait(&status);
        if(WIFEXITED(status)) printf("[Pai] processo %d terminou com resultado %d\n", terminated_pid, WEXITSTATUS(status));
        else{
            printf("erro\n");
        }
        return 0;
    }
}
