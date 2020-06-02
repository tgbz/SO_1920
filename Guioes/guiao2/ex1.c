#include<unistd.h> /*chamadas ao sistema : defs e decls essenciais*/
#include<sys/wait.h>
#include<stdio.h>

//Implementar um programa que imprima o seu identificador de processo e do seu pai. Comprove - invocando o comando ps- que o pai do seu processo é o interpretador de comandos que utilizou para o executar.

int main(int argc, char const *argv[]){
    printf("pid: %d\n", getpid());
    printf("pid pai: %d\n", getppid);
}
