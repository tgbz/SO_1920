#include <stdio.h>
#include <stdlib.h>
#include "Programa.h"

int main (int argc, char* argv[]){

    int size = 10;

    int vector[size];

    fill(vector, size, 5);

    // debug
    // for (int i = 0; i < size; i++) printf("%d\n", vector[i]);

    int teste = find(vector, size, 4);

    // debug
    // printf("%d\n", teste);

    return 0;
}
