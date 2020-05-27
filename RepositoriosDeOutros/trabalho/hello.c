#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* usadas para o open() */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* usada para o close() */
/* usada para o fork() */
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>

char *calculaSha1Sum(char* nomeFicheiro){
	int fd[2], nBytesLidos;
	char buff[4096];
	char *str;
  pipe(fd);
	if(fork()==0){
	  dup2(fd[1],1);
		close(fd[0]);
		close(fd[1]);
    execlp("sha1sum", "sha1sum", nomeFicheiro, NULL);
  }
  else{
    close(fd[1]);
    nBytesLidos = read(fd[0], buff, 4096);
    str = (char*)malloc(sizeof(char) * nBytesLidos + 1);
    if(str){
      strcpy(str, strtok(buff, "\r\n"));
    }
  }
	return str;
}

int backupCmd(int argc, char* argv[]){
  int i, fd;
  char *sha1, *path;
  if (argc == 3){
    fd = open(argv[2], O_RDONLY);
      if (fd == -1){
        printf("Erro ao abrir Ficheiro\n");
      }
      else{
        sha1 = calculaSha1Sum(argv[2]);
        sha1 = strtok(sha1, " ");
        path = strtok(NULL, " ");
        printf("Resultado SHA1SUM: %s - Ficheiro: %s\n", sha1, path);
        close(fd);
      }
  }
  
  else{
    for (i=2; i<argc; i++){
      fd = open(argv[i], O_RDONLY);
      if (fd == -1){
        printf("Erro ao abrir Ficheiro\n");
      }
      else{
        sha1 = calculaSha1Sum(argv[i]);
        sha1 = strtok(sha1, " ");
        path = strtok(NULL, " ");
        printf("Resultado SHA1SUM: %s - Ficheiro: %s\n", sha1, path);
        printf("inicia copia do ficheiro %s para o servidor.\n\n", argv[i]);
        close(fd);
      }
    }
  }
  
  return 0;
}

int restoreCmd(int argc, char *argv[]){
  int i, fd;
  char *sha1, *path;
  if (argc == 3){
    fd = open(argv[2], O_RDONLY);
    if (fd == -1){
      printf("Erro ao abrir Ficheiro\n");
    }
    else{
      sha1 = calculaSha1Sum(argv[2]);
      sha1 = strtok(sha1, " ");
      path = strtok(NULL, " ");
      printf("Resultado SHA1SUM: %s - Ficheiro: %s\n", sha1, path);
      close(fd);
    }
  }
  
  else{
    for (i=2; i<argc; i++){
      fd = open(argv[i], O_RDONLY);
      if (fd == -1){
        printf("Erro ao abrir Ficheiro\n");
      }
      else{
        sha1 = calculaSha1Sum(argv[i]);
        sha1 = strtok(sha1, " ");
        path = strtok(NULL, " ");
        printf("Resultado SHA1SUM: %s - Ficheiro: %s\n", sha1, path);
        printf("inicia reposicao do ficheiro %s do servidor.\n\n", argv[i]);
        close(fd);
      }
    }
  }
  
  return 0;
}
    
int main(int argc, char *argv[]){
  if (argc<=2){
      printf("Modo de uso:\n");
      printf(" sobucli backup *.txt\n");
      printf(" sobucli restore a.txt\n");
  }
  else{
    /*comando backup */
  	if(strcmp(argv[1], "backup")==0){
      backupCmd(argc, argv);
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
