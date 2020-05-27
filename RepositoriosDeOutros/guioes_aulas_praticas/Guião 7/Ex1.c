#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <unistd.h>
#include <signal.h>


unsigned int seconds = 0;
unsigned int numCtrlC = 0; // contador de CTRL+C


void sigint_handler (int signum) {
    numCtrlC ++;
    printf("\n[received %d] %d seconds passed\n", signum, seconds);
}


void sigquit_handler (int signum) {
    printf("\n[received %d] CRTL+C called %d times\n", signum, numCtrlC);
    exit(0); // terminar o programa
}


void sigalrm_handler (int signum) {
    seconds++;
    alarm(1);
}


int main (int argc, char* argv[]) {

    // Registar handlers de sinais SIGINT, SIGQUIT e SIGALRM

    signal(SIGINT, sigint_handler);
    signal(SIGQUIT, sigquit_handler);
    signal(SIGALRM, sigalrm_handler);

    alarm(1);

    while (1){
        pause();
        printf("executing ...\n");
    }

    return 0;
}