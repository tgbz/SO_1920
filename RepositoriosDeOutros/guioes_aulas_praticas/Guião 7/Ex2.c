#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <unistd.h>
#include <signal.h> 


int grep (char* word, char* file, int i) {

    int pid;
    
    if ((pid = fork()) == 0){

        printf("[Process %d] pid = %d | file = %s\n", i, getpid(), file);

        if (execlp("grep", "grep", word, file, NULL) < 0) perror("exec");

        _exit(-1);
    }

    return pid;
}


int main (int argc, char* argv[]){

    if (argc < 3){ 
        printf("Insuficient Arguments!\n");
        return 1;
    }

    int filesCount = argc - 2;
    char ** files = argv + 2;
    int pids [filesCount];

    for (int i = 0; i < filesCount; i++){
        pids[i] = grep(argv[1], files[i], i);
    }

    int found = 0;
    int status;
    int terminated_pid;
    int pid_found = -1;

    while (!found && (terminated_pid = wait(&status)) > 0){

        // grep retorna 0 quando não encontra nada

        if (WIFEXITED(status)){

            switch (WEXITSTATUS(status)){

                case 0:
                    printf("[process %d] found the word\n", terminated_pid);
                    found = 1;
                    pid_found = terminated_pid;
                    break;

                case 1:
                    printf("[process %d] word was not found\n", terminated_pid);
                    break;

                default:
                    break;
            }
        }
    }

    if (found == 1){

        for (int i = 0; i < filesCount; i++){

            if (pids[i] != pid_found){ 
                
                printf("killing process %d\n", pids[i]);
                // evitar a possibilidade de um kill -1
                if (pids[i] > 0) kill(pids[i], SIGKILL);

                // mostrar que o processo foi interrompido
                waitpid(pids[i], &status, 0);
                if (!WIFEXITED(status)) printf("process %d was interrupted\n", pids[i]);
                else printf("process %d ended correctly already\n", pids[i]);
            }
        }
    }

    return !found;
}

/*
Testes:
./a.out word file1 file2 file3 ...
*/