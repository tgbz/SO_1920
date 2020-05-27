#include <stdio.h>
#include <stdlib.h>
#include "Programa.h"

int vector[10];

int main (int argc, char* argv[]){

    fill(vector, 10, 5);

    // debug
    // for (int i = 0; i < size; i++) printf("%d\n", vector[i]);

    int teste = find(vector, 10, 5);

    // debug
    // printf("%d\n", teste);

    return 0;
}