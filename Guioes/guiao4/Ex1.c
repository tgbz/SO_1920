#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <fcntl.h>

//Escrever um programa que redirecione o descritor associado a seu standard input para o ficheiro
//etc/passwd, e o standard output e error respectivamente para a saida.txt e erros.txt.
//Experimente ler uma linha a partir do seu standard input e repeti-la nos descritores de output ///e error usando directamente read() e write(). Experimente também realizar as mesmas opeações através de funções da biblioteca de C, por exemplo fgets(), printf() ou fprintf(). Em todas as situações, repare no conteúdo dos ficheiros criados. Note que depois de cada redireccionamento, os descritores dos ficheiros abertos deixam de ser necessários, e como tal é boa prática fechá-los de imediato.


int main(int argc, char const *argv[])
{
    int input_fd = open("etc/passwd", O_RDONLY);
    int output_fd = open("saida.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666);
    int error_fd = open ("erros.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666);

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
        while(read(0, &buffer, 1)!=0){
            linha[i]=buffer;
            i++;
            if(buffer=='\n') break;
        }
        write(1,linha,i);
        write(2,linha,i);
        printf("linha escrita!\n");
        fflush(stdout);
        i=0;
        numLinhas --;
    }
    return 0;
}
