#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <fcntl.h>

//Implemente uma versão simplificada da função system().
//Ao contrário da função original, não tente suportar qualquer tipo de redireccionamento
//Ou composição/encadeamento de programas executáveis. O único argumento deverá
//Ser uma string que especifica um programa executável e uma eventual lista de argumentos.
//Procure que o comportamento e valor de retorno da sua função sejam compatíveis com a original.

int mysystem (char* command){
    char* exec_args[20];
    char* string;
    int i = 0;

    string = strtok(command, " ");
    //Separa a string por espaços
    while (string!= NULL){
        exec_args[i]=string;
        string=strtok(NULL," ");
        i++;
    }
    exec_args[i] = NULL;

    pid_t pid;
    int exec_ret, status;

    if((pid= fork())==0){
        exec_ret = execvp(exec_args[0], exec_args);
        _exit(exec_ret);
    }
    else{
        if(pid!=-1){
            pid_t terminated_pid = wait(&status);
            if(WIFEXITED(status)) return (WIFEXITED(status));
            else return -1;
        }
        else return -1;
    }
}

int main(int argc, char const *argv[])
{
    char command[]="ls -l -a -h";
    int ret = mysystem(command);
    printf("Valor de retorno: %d\n", ret);
    return 0;
}
