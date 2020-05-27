#include <stdio.h>
#include <unistd.h> 

int main (int argc, char** argv){

    int ret;

    printf("Mensagem 1\n");

    ret = execl("/bin/ls", "/bin/ls", "-l", NULL);

    printf("Mensagem 2\n"); // Não imprime porque troca o programa atual

    return 0;
}