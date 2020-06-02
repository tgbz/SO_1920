#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <fcntl.h>

//Modifique novamente o programa inicial de modo a que seja executado o comando wc, sem argumentos, depois
//do redirrecionamento dos descritores de entrada e saída. Note que, mais uma vez, as associações - e redirecionamentos
// de descritores de ficheiros são preservados pela primitiva exec()

int main(int argc, char const *argv[])
{
    int input_fd = open("/etc/passwd", O_RDONLY);
    int output_fd = open("saida.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666);
    int error_fd = open("erros.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666);

    setbuf(stdout, NULL); // escrever em ficheiro e não em memória

    dup2(input_fd,0);
    dup2(output_fd,1);
    dup2(error_fd,2);

    close(input_fd);
    close(output_fd);
    close(error_fd);

    execlp("wc", "/bin/wc", NULL);
    return 0;
}
