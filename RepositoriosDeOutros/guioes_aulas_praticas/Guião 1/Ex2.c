#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>    // chamadas ao sistema: defs e decls essenciais 
#include <fcntl.h>     // O_RDONLY, O_WRONLY, O_CREAT, O_*

#define SIZE 1024

int main (int argc, char* argv[]){

    char buffer[SIZE];

    int res = 0;

    while ((res = read(0, buffer, SIZE)) != 0){
        write (1, buffer, res);
    }

    return 0;
}
