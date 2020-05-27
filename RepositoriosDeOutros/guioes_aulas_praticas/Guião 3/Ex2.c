#include <stdio.h>
#include <unistd.h> 
#include <sys/wait.h>

int main (){

    pid_t pid;
    int status;

    if ((pid = fork()) == 0){
        execlp("ls", "ls", "-l", NULL);
        _exit(0);
    }
    else {
        pid_t terminated_pid = wait(&status);
        printf("[Pai] process %d terminated with result %d\n", terminated_pid, WEXITSTATUS(status));
    }

    return 0;
}