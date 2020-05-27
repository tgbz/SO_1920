#define _POSIX_SOURCE
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/types.h>
#include <signal.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/wait.h>

#include "enviaFicheiros.h"
#include "exec.h"

#define tamCMD 32

void sigHandler(int s){
  switch (s){
    case SIGUSR1:
      printf("Sinal OK Recebido.\n");
    break;
    case SIGUSR2:
      printf("Sinal KO Recebido.\n");
      kill(getpid(), SIGQUIT);
    break;
    default:
    break;  
  }
}

int enviaComando(char* FIFO, char TIPO, char* ficheiro){
  int pid, fd, i, status;
  char comando[32];

  signal(SIGUSR1,sigHandler);
  signal(SIGUSR2,sigHandler);
  pid = getpid();
  i = sprintf(comando, "%d %c %s", pid, TIPO, ficheiro);

  if(fork()==0){
    fd = open(FIFO, O_WRONLY);
    if(fd > 0){
      write(fd, comando, i); /*comando[tamCMD]*/
      close(fd);
      _exit(0);
      printf("Comando \"%s\" com tamanho %d foi enviado com sucesso!\n", comando, i);
    }
    else{
      printf("erro open FIFO.\n");
    }
  }
  else {
    /*pause();*/
    wait(&status);
    printf("Comando \"%s\" com tamanho %d foi enviado com sucesso!\n", comando, i);
  }
  return 0;
}

int backupCmd(int argc, char* argv[]){
  int i, fd;
  char *sha1, *path;
  char str1[100], str2[100];
  int pid;
  pid=getpid();
  for (i=2; i<argc; i++){
    fd = open(argv[i], O_RDONLY);
    if (fd == -1){
      printf("Erro ao abrir Ficheiro\n");
    }
    else{
      close(fd);
      sha1 = calculaSha1Sum(argv[i]);
      sha1 = strtok(sha1, " ");
      path = strtok(NULL, "\n");
      printf("Resultado SHA1SUM: %s - Ficheiro: %s\n", sha1, path);
      printf("inicia copia do ficheiro %s para o servidor.\n", argv[i]);
      sprintf(str1, "teste/fifos/%dC", pid);
      sprintf(str2, "teste/fifos/%dD", pid);
      enviaComando(str1, 'B', argv[i]);
      iniciaTx(str2, argv[i]);
    }
  }
  enviaComando(str1, 'S', "aaa");
  return 0;
}

int restoreCmd(int argc, char *argv[]){
  int i, fd;
  char *sha1, *path;
  char str1[100];
  int pid;
  pid=getpid();
  for (i=2; i<argc; i++){
    fd = open(argv[i], O_RDONLY);
    if (fd == -1){
      printf("Erro ao abrir Ficheiro\n");
    }
    else{
      close(fd);
      sha1 = calculaSha1Sum(argv[i]);
      sha1 = strtok(sha1, " ");
      path = strtok(NULL, "\n");
      printf("Resultado SHA1SUM: %s - Ficheiro: %s\n", sha1, path);
      printf("inicia reposicao do ficheiro %s do servidor.\n", argv[i]);
      sprintf(str1, "teste/fifos/%dD", pid);
      iniciaRx(str1, path);
    }
  }
  return 0;
}

int IniciaSessao(char* FIFO, char TIPO){
  int pid, fd, i, status, erro;
  char comando[32];

  signal(SIGUSR1,sigHandler);
  signal(SIGUSR2,sigHandler);
  pid = getpid();
  i = sprintf(comando, "%d %c", pid, TIPO);
  
  if(fork()==0){
    fd = open(FIFO, O_WRONLY);
    if(fd > 0){
      write(fd, comando, i); /*comando[tamCMD]*/
      close(fd);
      _exit(0);
      printf("Comando \"%s\" com tamanho %d foi enviado com sucesso!\n", comando, i);
    }
    else{
      printf("erro open FIFO.\n");
    }
  }
  else {
    pause();
    wait(&status);
    printf("Comando \"%s\" com tamanho %d foi enviado com sucesso!\n", comando, i);
  }
  return 0;
}

int comandoSessao(char* FIFO, char TIPO){
  int fd, tam, erro;
  erro = 0;
  fd = open(FIFO, O_WRONLY);
  if(fd > 0){
    tam = sprintf(comando, "%d %c", pid, TIPO);
    write(fd, comando, tam);
  }
  else{
    erro = 1; /*erro ao abrir FIFO*/
  }
  close(fd);
  return erro;
}

int main(int argc, char *argv[]){
  int erro;
  char *sobusrv = "teste/fifos/srv"; /*Named Pipe do servidor*/

  erro = 0;

  if (argc<=2){
      printf("Modo de uso:\n");
      printf(" sobucli backup *.txt\n");
      printf(" sobucli restore a.txt\n");
  }
  else{
    /*comando backup */
  	if(strcmp(argv[1], "backup")==0){
      erro = comandoSessao(sobusrv, 'B');
      if (erro == 0){
        pause();
        backupCmd(argc, argv);
      }
      else {
        printf("Erro ao enviar comando Sessao\n");
      }
  	}
  	
    /*comando restore*/
  	else if (strcmp(argv[1], "restore")==0){
  	  restoreCmd(argc, argv);
    }
    
    /* outros comandos */
/*    
    else if (strcmp(argv[1], "outros comandos")==0){
    }
*/
  }
  return 0;
}

