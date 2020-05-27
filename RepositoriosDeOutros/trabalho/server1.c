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

#include "enviaFicheiros.h"
#include "exec.h"S

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

/******************************************************************/

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
  str = strtok(comando, "\r\n");
  pid = atoi(strtok(str, " "));
  cmd = strtok(NULL, "\0");
  printf("Recebeu do pid %d o comando %s\n", pid, cmd);
  return pid;
}


/******************************************************************/

int criaServer(int ID){	
  int fd, loopT, pid, i, flag;
  char servidorFIFODados[128];
  char servidorFIFOComando[128];
  char buffer[128];
  char *str, *ficheiro, *cmd;
  char *servidorFIFOPATH = "teste/fifos/";
  
  sprintf(servidorFIFODados, "%s%dD", servidorFIFOPATH, ID);
  sprintf(servidorFIFOComando, "%s%dC", servidorFIFOPATH, ID);
  
  loopT = 1;
  flag = 1;
  pid = 0;
  signal(SIGINT,sigHandler);
  
  if ((mkfifo(servidorFIFOComando, 0666)) == 0) {
     kill(ID,SIGUSR1); /*cliente pode comecar a copiar ficheiros */
     printf("arrancou servidor com id: %d\n", getpid());
  }
  else {
    printf("Erro de FIFO");
    loopT = 0;
  }
  
  while(loopT){
    fd = open(servidorFIFOComando, O_RDONLY);
/*    for (i=0; i<128; i++) buffer[i] = '\0'; */
    while (read(fd, buffer, 128) > 0) {
      str = strtok(buffer, "\r\n");
      pid = atoi(strtok(str, " "));
      cmd = strtok(NULL, " ");
      ficheiro = strtok(NULL, "\0");
    }
    if(strcmp(cmd, "S") == 0){
      loopT=0;
      flag=0;
      printf("######## Fichei servidor ########\n");
    }
    if (pid!=0){
      close(fd);
      kill(pid,SIGUSR1);
      printf("Recebi do pid %d o comando %s para o ficheiro %s\n", pid, cmd, ficheiro);
      if (flag)
        executaComando(cmd, pid, ficheiro);
      printf("Enviou Sinal %d para o pid %d\n", SIGUSR1, pid);
      pid = 0;
    }
  }
  unlink(servidorFIFOComando);
  return 0;
}

/******************************************************************/

int main(){	
  int fd, loopT, pid, i, maxCli, status;
  char *servidorFIFOPATH = "teste/fifos/srv";
  char buffer[128];
  int tabelaLigacoes[2][5]; /* pid do cliente / pid do fork */

  loopT = 1;
  signal(SIGINT,sigHandler);
  maxCli=0;
  pid=0;
    
  if ((mkfifo(servidorFIFOPATH, 0666)) == 0) {
     printf("arrancou servidor com id: %d\n", getpid());
  }
  else {
    printf("Erro de FIFO");
    loopT = 0;
  }
  
  while(loopT){
    fd = open(servidorFIFOPATH, O_RDONLY);
    for (i=0; i<128; i++) buffer[i] = '\0';
    while (read(fd, buffer, 128) > 0) {
      pid = leComandoPid(buffer);
    }
    close(fd);
    if (pid != 0 && maxCli < 5){
      maxCli++;
      tabelaLigacoes[0][maxCli-1] = pid;
      if((tabelaLigacoes[0][maxCli-1] = fork())==0){
        criaServer(pid);
        _exit(0);
      }
      else {
        wait(&status);
        maxCli--;
      }
    }
    else {
      kill(pid,SIGUSR2);
    }
  }
  unlink(servidorFIFOPATH);
  return 0;
}

