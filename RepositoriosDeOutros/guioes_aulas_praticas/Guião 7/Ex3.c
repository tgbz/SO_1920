#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <unistd.h>
#include <signal.h> 


int pidsCount;
int* pids;


void timeout_handler (int signum) {

    for (int i = 0; i < pidsCount; i++){

        printf("process %d in timeout\n", pids[i]);

        if (pids[i] > 0) kill(pids[i], SIGKILL);
    }
}


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

    if (signal(SIGALRM, timeout_handler) < 0){
        perror("signal SIGALRM");
        exit(-1);
    }

    int filesCount = argc - 2;
    pidsCount = filesCount;
    pids = malloc(sizeof(int) * pidsCount);
    char ** files = argv + 2;

    for (int i = 0; i < filesCount; i++){
        pids[i] = grep(argv[1], files[i], i);
    }

    alarm(10);

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
        else printf("process %d was interrupted\n", terminated_pid);
    }

    if (found == 1){

        for (int i = 0; i < filesCount; i++){

            if (pids[i] != pid_found){ 
                
                printf("killing process %d\n", pids[i]);
                kill(pids[i], SIGKILL);

                waitpid(pids[i], &status, 0);
                if (!WIFEXITED(status)) printf("process %d was interrupted\n", pids[i]);
                else printf("process %d ended correctly already\n", pids[i]);
            }
        }
    }

    free(pids);
    return !found;
}

/*
Testes:
./a.out word file1 file2 file3 ...
*/