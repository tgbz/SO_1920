/*comando Kill - compiler warning: implicit declaration of function ‘kill’ */
/* #define _POSIX_SOURCE */

/* open() precisa das seguintes libs */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* mkfifo() precisa das seguintes libs */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> /* Definicao das constantes AT_* */
#include <sys/stat.h>

/* close(), write(), read() precisam das seguintes libs */
#include <unistd.h>

/* funcao kill() precisa das seguintes libs*/
/* #include <sys/types.h> */
/* #include <signal.h> */

#include <stdio.h>
#include <stdlib.h>
/*#include <string.h>*/

int iniciaTx(char* FIFO, char* ficheiro){ /*envia ficheiro pelo fifo*/
  int fdFicheiro, fdFIFO, tamMax, erro;
  char buffer[4096];
  erro = 0;
  if ((fdFicheiro = open(ficheiro, O_RDONLY)) > 0){
    if ((fdFIFO = open(FIFO, O_WRONLY)) > 0){
      printf("IniciaTx() FIFO: %s\n", FIFO);
      printf("IniciaTx() File: %s\n", ficheiro);
      while ((tamMax=read(fdFicheiro, buffer, 4096)) > 0){
        write(fdFIFO, buffer, tamMax);
      }
      close(fdFIFO);	
      close(fdFicheiro);
    }
    else {
      erro = 2; /*erro no open do FIFO*/
    }
  }
  else {
    erro = 1; /*erro no open do FICHEIRO*/
  }
  return erro;
}

int iniciaRx(char* FIFO, char* ficheiro){/*recebe ficheiro pelo FIFO */
  int fdFicheiro, fdFIFO, tamMax, erro;
  char buffer[4096];
  erro = 0;
  printf("RX FIFO: %s\n", FIFO);
  printf("RX File: %s\n", ficheiro);
  if (mkfifo(FIFO, 0422) == 0){ /* permissoes do fifo - R-- -W- -W- */
    if((fdFicheiro = open(ficheiro, O_WRONLY | O_CREAT, 0644))>0){ /*permissoes do ficheiro devem ser colocadas iguais ás do utilizador */
      if((fdFIFO = open(FIFO, O_RDONLY))>0){ /*stdin*/
        while ((tamMax=read(fdFIFO, buffer, 4096)) > 0){
          write(fdFicheiro, buffer, tamMax);
        }
        close(fdFicheiro);
        close(fdFIFO);
        unlink(FIFO);
      }
      else {
        erro = 3; /*erro no open do FIFO */
      }
    }
    else {
      erro = 2; /*erro no open do Ficheiro */
    }
  }
  else {
    erro = 1; /*erro no make do fifo */
  }
  return erro;
}

