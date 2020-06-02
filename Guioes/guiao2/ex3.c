#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
    pid_t pid;
    int status;
    int numProcessos = 10;

    for (int i = 1; i <= numProcessos; i++){
        if((pid = fork()) == 0){
            printf("[Processo %d] pid: %d / pai : %d /pai : %d\n\n", i, getpid(), getppid());
            _exit(i);
        }
        else {
            pid_t terminated_pid = wait(&status);
            printf("[Pai] o processo %d saiu com um código de saída %d\n\n", terminated_pid, WEXITSTATUS(status));
        }
    }
    return 0;
}