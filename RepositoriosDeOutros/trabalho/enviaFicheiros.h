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

int iniciaTx(char* FIFO, char* ficheiro); /*envia ficheiro pelo fifo*/

int iniciaRx(char* FIFO, char* ficheiro); /*recebe ficheiro pelo FIFO*/
