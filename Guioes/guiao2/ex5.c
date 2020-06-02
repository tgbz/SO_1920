#include <stdio.h>
#include <unistd.h>  
#include <sys/wait.h>

int main(int argc, char const *argv[])
{
    pid_t pid;
    int status;
    int numProcessos = 10;

    for (int i = 0; i <= numProcessos; i++){
        if ((pid=fork())==0){
            printf("[Processo %d] pid: %d / pai: %d\n", i, getpid(), getppid());
        }
        else{
            pid_t terminated_pid = wait(&status);
            if(WIFEXITED(status)){
                printf("[Pai] o processo %d saiu\n", terminated_pid);
            }
            else{ 
                printf("[Pai] o processo %d saiu\n", terminated_pid);
            }
             _exit(0);
        }
    }
    _exit(0);
    return 0;
}
