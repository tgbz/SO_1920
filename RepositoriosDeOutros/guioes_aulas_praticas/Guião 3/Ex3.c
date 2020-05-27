#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <fcntl.h>

int main (int argc, char** argv){

    for (int i = 0; i < argc; i++){
        write(1, argv[i], strlen(argv[i]));
        write(1, "\n", 1);
    }

    return 0;
}

/*
Testes:
gcc Ex3.c -o Ex3
./Ex3 Hello World
*/