#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  
#include <sys/wait.h>

int main (int argc, char* argv[]){

    if (argc < 2){
        printf("Insert number to find!\n");
        exit(-1);
    }

    int valor = atoi(argv[1]);
    int numLinhas = 100;
    int numColunas = 10000;
    int max = 10000;
    int **matrix;

    pid_t pid;
    int status;

    // Gerar a matriz

    printf("Generating numbers from 0 to %d ... ", max);
    matrix = (int**)malloc(sizeof(int*) * numLinhas);
    for (int i = 0; i < numLinhas; i++){
        matrix[i] = (int*)malloc(sizeof(int) * numColunas);
        for (int j = 0; j < numColunas; j++){
            matrix[i][j] = rand() % max;
        }
    }
    printf("Done\n");

    // Procurar na matriz

    for (int i = 0; i < numLinhas; i++){
        if ((pid = fork()) == 0){
            for(int j = 0; j < numColunas; j++){
                if (matrix[i][j] == valor) _exit(i); // se encontrar devolve ao pai a linha em que encontrou
            }
            _exit(-1); // não encontrou na linha
        }
    }

    // O Pai fica à escuta dos filhos

    for (int i = 0; i < numLinhas; i++){
        pid_t terminated_pid = wait(&status);
        if (WIFEXITED(status)) {
            // 0 <= status <= 254 -> encontrou
            if (WEXITSTATUS(status) < 255){
                printf("[Pai] process %d exited. found number at row %d\n", terminated_pid, WEXITSTATUS(status));
            }
            // status == 255 == -1 ->  não encontrou
            else {
                printf("[Pai] process %d exited. nothing found\n", terminated_pid);
            }
        }
        else printf("[Pai] process %d exited. something went wrong\n", terminated_pid);
    }

    return 0;
}