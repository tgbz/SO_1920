#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <sys/wait.h>

int main (int argc, char* argv[]){

    int p[2];
    char line[] = "linha de teste";
    char buffer[20];
    int status;

    if (pipe(p) == -1){ 
        perror("Pipe não foi criado!");
        return -1;
    }

    switch (fork()){

        case -1: 
            perror("Fork não foi efetuado!");
            return -1;

        case 0:
            // Código Filho
            close(p[1]);
            read(p[0], &buffer, sizeof(buffer));
            printf("[Filho] Li do pipe a mensagem: %s\n", buffer);
            close(p[0]);
            _exit(0);
        
        default:
            // Código Pai
            close(p[0]);
            //sleep(5);
            write(p[1], &line, sizeof(line));
            printf("[Pai] Escrevi no pipe a mensagem: %s\n", line);
            close(p[1]);
            wait(&status);
    }

    return 0;
}

/*
Inverter os papeis de modo à informação ser transmitida do filho para o pai -> trocar p[0] por p[1] e vice versa.
*/