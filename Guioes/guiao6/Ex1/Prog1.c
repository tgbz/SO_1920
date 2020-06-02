//Escreva três programas. O primeiro cria apenas um pipe com "fifo". 
//O segundo repete para este pipe todas as linhas de texto lidas 
// a partir do standard input.
//O terceiro programa deverá repetir para o seu standard output todas as linhas de texto
//lidas a partir deste pipe.
//Repare que ao contrário dos pipes anónimos
//o pipe corresponde a uma entrada no sistema de ficheiros, sujeito ao mesmo controlo de acesso
//aos ficheiros normais, e não obria à criação do pipe por um processo ascendente dos processos
//em comunicação.
//Aliás, a comunicação pode mesmo realizar-se entre processos de utilizadores distintos.
//Note ainda que tal como nos pipes anónimos, as operações de leitura e escrita no pipe oferecem
// um canal uniderrecional sob uma política FIFO e diluição da eventual fronteira das escritas. 
//No entanto, ao contrário dos pipes anónimos, a abertura para escrita de um pipe como nome 
//bloqueia até que um processo o abra para leitura, e vice-versa
#include <stdio.h>
#include <sys/types.h> 
#include <sys/stat.h>

int main(int argc, char* argv[])
{
    if(mkfifo("fifo",0777)==-1){
        perror("mkfifo");
    }
    return 0;
}
