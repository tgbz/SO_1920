#include <stdio.h>
#include <unistd.h>     // chamadas ao sistema: defs e decls essenciais
#include <sys/wait.h>   // chamadas wait*() e macros relacionadas


int main(){
    printf("pid: %d\n", getpid());
    printf("pid pai: %d\n", getppid());
    return 0;
}