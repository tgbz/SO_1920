#include"vector.h"

void fill(int* vector, int size, int value){
    for (int i=0; i<size;i++){
        *(vector + i) = value;
    }
}

int find(int* vector, int size, int value){
    int n = 0;
    for (int i = 0; i < size; ++i){
        if(*(vector + 1) == value){
            n++;
        }
    }
    return n;
}

