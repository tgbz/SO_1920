#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <fcntl.h>

//Modificar o programa anterior de modo a que, depois de realizar os redirrecionamentos
//Seja criado um novo processo que realize operações de leitura e escrita. Experimente ainda usar o comando cat no proceso filho para copiar a entrada
//Para a saída, tal como fez no guiao1. Observe o conteúdo dos ficheiros. Repare que o processo filho "nasce" com as mesmas associações de descritores de ficheiros que o processo pai
// 

int main(int argc, char const *argv[])
{
    int input_fd = open("/etc/passwd", O_RDONLY);
    int output_fd = open("saida.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666);
    int error_fd = open("erros.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666);

    dup2(input_fd, 0);
    dup2(output_fd, 1);
    dup2(error_fd,2);

    close(input_fd);
    close(output_fd);
    close(error_fd);

    int numLinhas = 12;
    char buffer;
    char linha[1024];
    int i = 0;

    pid_t pid;
    int status;

    if((pid=fork()) == 0){
        printf("Sou o filho %d\n", getpid());
        fflush(stdout);

        while(numLinhas > 0){
            while(read(0, &buffer, 1)!=0){
                linha[i] = buffer;
                i++;
                if(buffer=='\n') break;
            }
        write(1, linha, i);
        write(2, linha, i);
        printf("linha escrita!\n");
        fflush(stdout);
        i = 0;
        numLinhas --;
        }
        _exit(0);
    }
    else{
        printf("sou o pai %d\n", getpid());
        pid_t terminated_pid = wait(&status);
    }    
    return 0;
}
