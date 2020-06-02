#include <stdio.h>
#include <unistd.h>  
#include <sys/wait.h>

int main(int argc, char const *argv[])
{
    pid_t pid;

    if((pid = fork()) == 0){
        printf("[Filho] pid: %d\n", getpid());
        printf("[Filho] pid pai: %d \n \n", getppid());
        _exit(0);
    }
    else{
        sleep(3);
        printf("[PAI] pid: %d\n", getpid());
        printf("[PAI] pid filho: %d\n", pid);
        printf("[PAI] pid pai: %d\n", getppid());
    }
    return 0;
}
