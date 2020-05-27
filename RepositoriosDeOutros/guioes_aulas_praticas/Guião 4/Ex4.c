#include <stdio.h>
#include <string.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <fcntl.h>

// Guião 3 Exercício 6

int mysystem (char* command){

    char* exec_args[20];
    char* string;
    int i = 0;

    string = strtok(command, " "); 

    while (string != NULL){
        exec_args[i] = string;
        string = strtok(NULL, " ");
        i ++;
    }

    exec_args[i] = NULL;

    pid_t pid;
    int exec_ret, status;

    if ((pid = fork()) == 0){
        exec_ret = execvp(exec_args[0], exec_args);
        _exit(exec_ret);
    }
    else {
        if (pid != -1){
            pid_t terminated_pid = wait(&status);
            if (WIFEXITED(status)) return (WIFEXITED(status));
            else return -1;
        }
        else return -1;
    }
}

int main (int argc, char** argv){

    int commandIndex = 1;
    char command[1024];

    if (strcmp(argv[commandIndex], "-i") == 0){
        int input_fd = open(argv[commandIndex + 1], O_RDONLY);
        dup2(input_fd,0);
        close(input_fd);
        commandIndex += 2;
    }

    if (strcmp(argv[commandIndex], "-o") == 0){
        int output_fd = open(argv[commandIndex + 1], O_CREAT | O_TRUNC | O_WRONLY, 0666);
        dup2(output_fd,1);
        close(output_fd);
        commandIndex += 2;
    }

    setbuf(stdout, NULL); 

    bzero(command, 1024); // limpar o array

    for (int i = commandIndex; i < argc; i++){
        strcat(command, argv[i]);
        strcat(command, " ");
    }

    mysystem(command);

    return 0;
}

/* 
Testes:
./a.out -i Ex1.c -o saida.txt wc
./a.out -o saida.txt ls -l
./a.out ls -l
*/