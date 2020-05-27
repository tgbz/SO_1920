#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <sys/wait.h>

#define MAX_LINE_SIZE 1024

// Read Line

ssize_t readln (int fildes, void* buffer, ssize_t numBytes){

    ssize_t res = 0;
    int i = 0;

    while (i < numBytes && (res = read(fildes, &buffer[i], 1)) > 0){

        if (((char*)buffer)[i] == '\n') return (i + 1);
        i += res;
    }

    return i;
}

// Main

int main (int argc, char* argv[]){

    int p[2];
    char buffer[MAX_LINE_SIZE];
    int res;
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
            dup2(p[0], 0);
            close(p[0]);
            execlp("wc", "/bin/wc", NULL);
            _exit(0);
        
        default:
            // Código Pai
            close(p[0]);
            while ((res = readln(0, buffer, MAX_LINE_SIZE)) > 0){
                write(p[1], buffer, res);
            }
            close(p[1]);
            wait(&status);
            if (WIFEXITED(status)) printf("[Pai] o filho terminou com %d\n", WEXITSTATUS(status));
    }

    return 0;   
}

/* 
Testes:
./a.out
Hello World
...
^d
*/