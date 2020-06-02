#include <stdio.h>
#include <unistd.h>

//Programa que executa o comando ls -l. Note que no caso da execução ser bem sucedida
//Mais nenhuma outra instrução é executada do programa original.
int main(int argc, char const *argv[])
{
    int ret;

    printf("Mensagem 1\n");

    ret = execl("/bin/ls", "/bin/ls", "-l", NULL);

    printf("Mensagem 2\n"); //Não imprime porque o troca o programa actual

    return 0;
}
