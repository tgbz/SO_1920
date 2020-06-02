#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <sys/wait.h>

//Escreva um programa que crie um pipe anónimo e de seguida crie um processo filho.
//Relembre que as associações dos descritores de ficheiros são preservadas na criação do processo filho
//Experimente o pai enviar uma linha de texto através do descritor de escrita do pipe
//E o filho receber uma linha a partir do respectivo descritor de leitura.
//Note que a informação foi transferida do pai para o filho
//Experimente de seguida provocar um atraso do pai enviar a linha de texto (ex, sleep(5));
//Note agora que a leitura do filho bloqueia enquanto o pai não realizar a operação de escrita no pipe
//Experimente agora inverter os papeis de modo à informação ser transmitida do filho para o pai.

int main(int argc, char const *argv[])
{
    int p[2];
    char line[] = "linha de teste";
    char buffer[20];
    int status;

    if(pipe(p)== -1){
        perror("Pipe não foi criado!");
        return -1;
    }

    switch(fork()){
        case -1:
        perror("Fork não foi efetuado!");
        return -1;
        

        
        //Código para ao filho
        close(p[1]);
        read(p[0], &buffer, sizeof(buffer));
        printf("[Filho] Li do pipe a mensagem: %s\n", buffer);
        close(p[0]);
        _exit(0);

        case 0:
        //Código Pai
        close(p[0]);
        //sleep(5);
        write(p[1],&line,sizeof(line));
        printf("[Pai] Escrevi no pipe a mensagem: %s\n", line);
        close(p[1]);
        wait(&status);
    }
    return 0;
}
