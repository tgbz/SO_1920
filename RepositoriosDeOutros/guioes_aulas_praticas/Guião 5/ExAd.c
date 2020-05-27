#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <sys/wait.h>

int redirect (char* type, char* filename){

    int fd;

    if (strcmp(type, "<") == 0){
        fd = open(filename, O_RDONLY);
        dup2(fd,0);
        close(fd);
    }

    else if (strcmp(type,">") == 0){
        fd = open(filename, O_CREAT | O_TRUNC | O_WRONLY, 0666);
        dup2(fd,1);
        close(fd);
    }

    else if (strcmp(type,">>") == 0){
        fd = open(filename, O_CREAT | O_APPEND | O_WRONLY, 0666);
        dup2(fd,1);
        close(fd);
    }

    else if (strcmp(type,"2>") == 0){
        fd = open(filename, O_CREAT | O_TRUNC | O_WRONLY, 0666);
        dup2(fd,2);
        close(fd);
    }

    else {
        fd = open(filename, O_CREAT | O_APPEND | O_WRONLY, 0666);
        dup2(fd,2);
        close(fd);
    }

    return 0;
}