#define _POSIX_SOURCE /*comando Kill - compiler warning: implicit declaration of function ‘kill’ */
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>

void sigHandler(int s){
  switch (s){
    case SIGUSR1:
      printf("Sinal OK Recebido.\n");
    break;
    case SIGUSR2:
      printf("Sinal KO Recebido.\n");
    break;
    case SIGINT:
      unlink("teste/fifos/srv");
      kill(getpid(), SIGQUIT);
    break;
    default:
    break;  
  }
}

int iniciaTx(char* FIFO, char* ficheiro){
  int fdFicheiro, fdFIFO, tamMax;
  char buffer[4096];
  fdFicheiro = open(ficheiro, O_RDONLY);
  fdFIFO = open(FIFO, O_WRONLY);
  printf("TX FIFO: %s\n", FIFO);
  printf("TX File: %s\n", ficheiro);
  while ((tamMax=read(fdFicheiro, buffer, 4096)) > 0){
    write(fdFIFO, buffer, tamMax);
  }
  close(fdFIFO);
  close(fdFicheiro);
  return 0;
}

int iniciaRx(char* FIFO, char* ficheiro){
  int fdFicheiro, fdFIFO, tamMax;
  char buffer[4096];
  printf("RX FIFO: %s\n", FIFO);
  printf("RX File: %s\n", ficheiro);
  mkfifo(FIFO, 0666);
  fdFicheiro = open(ficheiro, O_WRONLY | O_CREAT, 0644); /*stdout*/
  fdFIFO = open(FIFO, O_RDONLY); /*stdin*/
  while ((tamMax=read(fdFIFO, buffer, 4096)) > 0){
    write(fdFicheiro, buffer, tamMax);
  }
  close(fdFicheiro);
  close(fdFIFO);
  unlink(FIFO);
  return 0;
}

void comandoB(char* FIFO, char* ficheiro){
  iniciaRx(FIFO, ficheiro);
}

void comandoR(char* FIFO, char* ficheiro){
  iniciaTx(FIFO, ficheiro);
}

int executaComando(char *cmd, int pid, char *ficheiro){
  char fifo[128];
  char file[128];
  char path[] = "teste/fifos/";
  char pathFicheiro[] = "teste/serverfiles/";
  sprintf(fifo, "%s%dD", path, pid);
  sprintf(file, "%s%s", pathFicheiro, ficheiro);
  if (strcmp(cmd,"B")==0){
    comandoB(fifo, file);
  }
  else if (strcmp(cmd,"R")==0)
    comandoR(fifo, file);
  else {
    printf("Comando Desconhecido\n");
  }
  return 0;
}
  
int leComandoPid(char *comando){ /*retorna o pid do processo filho*/
  int pid;
  char *cmd;
  char *str;
  char *ficheiro;
  str = (char*)malloc(sizeof(char) * (strlen(comando)+1));
  cmd = (char*)malloc(sizeof(char) * (strlen(comando)+1));
  str = strtok(comando, "\r\n");
  pid = atoi(strtok(str, " "));
  cmd = strtok(NULL, " ");
  ficheiro = strtok(NULL, "\0");
  printf("Recebeu do pid %d o comando %s para o ficheiro %s\n", pid, cmd, ficheiro);
  return pid;
}

/* antiga main */
int criaServer(char *FIFO){	
  int fd, loopT, pid, i;
  char servidorFIFODados[128];
  char servidorFIFOComando[128];
  char buffer[128];
  char *str, *ficheiro, *cmd;
  char *servidorFIFOPATH = "teste/fifos/";
  
  sprintf(servidorFIFODados, "%s%dD", servidorFIFOPATH, pid);
  sprintf(servidorFIFOComando, "%s%dC", servidorFIFOPATH, pid);
  
  loopT = 1;
  signal(SIGINT,sigHandler);
  
  if ((mkfifo(servidorFIFOComando, 0666)) == 0) {
     printf("arrancou servidor com id: %d\n", getpid());
  }
  else {
    printf("Erro de FIFO");
    loopT = 0;
  }
  
  while(loopT){
    fd = open(servidorFIFOComando, O_RDONLY);
    for (i=0; i<128; i++) buffer[i] = '\0';
    while (read(fd, buffer, 128) > 0) {
      str = strtok(buffer, "\r\n");
      pid = atoi(strtok(str, " "));
      cmd = strtok(NULL, " ");
      ficheiro = strtok(NULL, "\0");
    }
    close(fd);
    kill(pid,SIGUSR1);
    printf("Recebi do pid %d o comando %s para o ficheiro %s\n", pid, cmd, ficheiro);
    executaComando(cmd, pid, ficheiro);
    printf("Enviou Sinal %d para o pid %d\n", SIGUSR1, pid);
  }
  unlink(servidorFIFOComando);
  return 0;
}

