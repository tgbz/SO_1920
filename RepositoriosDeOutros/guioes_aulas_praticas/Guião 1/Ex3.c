#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>   
#include <fcntl.h>

#define SIZE 1024

ssize_t readln (int fd, char *line, size_t size){

    ssize_t res = 0;
    int i = 0;

    while (i < size && (res = read(fd, &line[i], 1)) > 0){
        if (line[i] == '\n'){
            i += res;
            return 1;
        }
        i += res;
    }
    return 0;
}

int main (int argc, char* argv[]){

    char* line = malloc(sizeof(char)*SIZE);
    int fd = open(argv[1], O_RDONLY);
    readln(fd, line, SIZE);
    printf("%s", line);
}

/*
Testes:
gcc Ex3.c
./a.out texto.txt
*/