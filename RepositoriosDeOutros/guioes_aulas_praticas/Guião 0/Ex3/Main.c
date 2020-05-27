#include <stdio.h>
#include <stdlib.h>
#include "Programa.h"

int main (int argc, char* argv[]){

    int size = 5000;

    int* vector = (int*)malloc(size*sizeof(int));

    int inicio = 1000;
    int fim = 2000;

    fill(vector+inicio, fim-inicio+1, 5);

    int teste = find(vector, size, 4);

    // debug
    // for (int i = 0; i < size; i++) printf("%d\n", vector[i]);
    // printf("%d\n", teste);

    free(vector);

    return 0;
}