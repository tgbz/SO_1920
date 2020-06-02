#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
//Implemente um programa semelhante ao anterior que execute ls -l
//Mas gora no contexto de um processo filho
int main(int argc, char const *argv[])
{
    pid_t pid;
    int status;

    if ((pid = fork())==0){
        execlp("ls","ls","-l", NULL);
        _exit(0);
    }
    else {
        pid_t terminated_pid = wait(&status);
        printf("[Pai] process %d terminated with result %d \n", terminated_pid, WEXITSTATUS(status));
    }
    return 0;
}
