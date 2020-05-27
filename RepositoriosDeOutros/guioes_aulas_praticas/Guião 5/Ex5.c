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

    char command1[] = "grep -v ˆ# /etc/passwd";
    char command2[] = "cut -f7 -d:";
    char command3[] = "uniq";
    char command4[] = "wc -l";

    char* commands[4];
    commands[0] = command1;
    commands[1] = command2;
    commands[2] = command3;
    commands[3] = command4;

    int numCommands = 4;
    int pipes[numCommands - 1][2];
    int status[numCommands];

    for (int i = 0; i < numCommands; i++){

        if (i == 0){

            if (pipe(pipes[i]) != 0){ 
                perror("Pipe não foi criado!");
                return -1;
            }

            switch (fork()){

                case -1: 
                    perror("Fork não foi efetuado!");
                    return -1;

                case 0:
                    // Código Primeiro Filho
                    close(pipes[i][0]);
                    dup2(pipes[i][1], 1); // ligar stdout do primeiro comando ao extremo de escrita do primeiro pipe
                    close(pipes[i][1]);
                    exec_command(commands[i]);
                    _exit(0);
        
            default:
                // Código Pai
                close(pipes[i][1]);
            }
        }

        else if (i == numCommands - 1){

            switch (fork()){

                case -1: 
                    perror("Fork não foi efetuado!");
                    return -1;

                case 0:
                    // Código Último Filho
                    dup2(pipes[i-1][0], 0);
                    close(pipes[i-1][0]);
                    exec_command(commands[i]);
                    _exit(0);
        
            default:
                // Código Pai
                close(pipes[i-1][0]);
            }
        }

        else {

            if (pipe(pipes[i]) != 0){ 
                perror("Pipe não foi criado!");
                return -1;
            }

            switch (fork()){

                case -1: 
                    perror("Fork não foi efetuado!");
                    return -1;

                case 0:
                    // Código Filhos do Meio
                    close(pipes[i][0]);
                    dup2(pipes[i][1], 1); // escreve para o pipe i
                    close(pipes[i][1]);
                    dup2(pipes[i-1][0], 0); // lê do pipe i-1
                    close(pipes[i-1][0]);
                    exec_command(commands[i]);
                    _exit(0);
        
            default:
                // Código Pai
                close(pipes[i][1]);
                close(pipes[i-1][0]);
            }
        }
    }

    for (int i = 0; i < numCommands; i++){
        wait(&status[i]);
    }

    return 0;
}