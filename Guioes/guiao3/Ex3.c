#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

//Implemente um programa que imprima a lista de argumentos recebidos na sua linha de comando
int main(int argc, char const *argv[]){
    for (int i = 0; i < argc; i++){
        write(1,argv[i],strlen(argv[i]));
        write(1,"\n",1);
    }
    return 0;
}
