#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <fcntl.h>

int main (int argc, char** argv){

    strcpy(argv[0], "exc");
    execv("Ex3", argv);

    return 0;
}

/*
Testes:
gcc Ex4.c -o Ex4
./Ex4 Hello World
*/