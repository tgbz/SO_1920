#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <sys/wait.h>

int main (int argc, char* argv[]){

    int p[2];
    char* line[] = {"linha 1\n", "linha 2\n", "linha 3\n", "linha 4\n", "linha 5\n"};
    int numLinhas = 5;
    char buffer;
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
            printf("[Filho] li do pipe a mensagem:\n");
            while(read(p[0], &buffer, 1) > 0){
                printf("%c", buffer);
            }
            close(p[0]);
            _exit(0);
        
        default:
            // Código Pai
            close(p[0]);
            //sleep(5);
            for (int i = 0; i < numLinhas; i++){
                write(p[1], line[i], sizeof(line[i]));
                printf("[Pai] Escrevi no pipe a mensagem: %s", line[i]);
            }
            close(p[1]);
            wait(&status);
    }

    return 0;
}