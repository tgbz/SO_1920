#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <fcntl.h>

int main (int argc, char** argv){

    int input_fd = open("/etc/passwd", O_RDONLY);
    int output_fd = open("saida.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666);
    int error_fd = open("erros.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666);

    dup2(input_fd, 0);
    dup2(output_fd, 1);
    dup2(error_fd, 2);

    close(input_fd);
    close(output_fd);
    close(error_fd);

    int numLinhas = 12;
    char buffer;
    char linha[1024];
    int i = 0;
    
    while (numLinhas > 0){
        while (read(0, &buffer, 1) != 0){
            linha[i] = buffer;
            i++;
            if (buffer == '\n') break;
        }
        write(1, linha, i);
        write(2, linha, i);
        printf("linha escrita!\n");
        fflush(stdout);
        i = 0;
        numLinhas --;
    }

    return 0;
}