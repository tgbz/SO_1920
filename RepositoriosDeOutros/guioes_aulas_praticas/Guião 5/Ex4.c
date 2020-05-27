#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <sys/wait.h>

// Executar Comando

int exec_command (char* command){
    
    char* exec_args[20];
    char* string;
    int ret = 0;
    int i = 0;

    string = strtok(command, " ");

    while (string != NULL){
        exec_args[i] = string;
        string = strtok(NULL, " ");
        i++;
    }

    exec_args[i] = NULL;

    ret = execvp(exec_args[0], exec_args);

    return ret;
}

// Main

int main (int argc, char* argv[]){

    char command1[] = "ls /etc";
    char command2[] = "wc -l";

    int p[2];
    int status[2];

    if (pipe(p) == -1){ 
        perror("Pipe não foi criado!");
        return -1;
    }

    switch (fork()){

        case -1: 
            perror("Fork não foi efetuado!");
            return -1;

        case 0:
            // Código Filho 1
            close(p[0]);
            dup2(p[1], 1); // ligar o stdout do primeiro comando ao estremo de escrita do pipe
            close(p[1]);
            exec_command(command1);
            _exit(0);
        
        default:
            // Código Pai
            close(p[1]);
    }

    switch (fork()){

        case -1: 
            perror("Fork não foi efetuado!");
            return -1;

        case 0:
            // Código Filho 2
            dup2(p[0], 0); // ligar o stdin ao extremo de leitura do pipe
            close(p[0]);
            exec_command(command2);
            _exit(0);
        
        default:
            // Código Pai
            close(p[0]);
    }

    for (int i = 0; i < 2; i++){
        wait(&status[i]);
    }

    return 0; 
}