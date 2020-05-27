#include <stdio.h>
#include <unistd.h>  
#include <sys/wait.h>

int main(){

    pid_t pid;

    if ((pid = fork()) == 0){
        printf("[Filho] pid: %d\n", getpid());
        printf("[Filho] pid pai: %d\n\n", getppid());
        _exit(0);
    }
    
    else {
        sleep(3);
        printf("[Pai] pid: %d\n", getpid());
        printf("[Pai] pid filho: %d\n", pid);
        printf("[Pai] pid pai: %d\n", getppid());
    }

    return 0;
}