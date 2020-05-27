/* pipe() */
#define _GNU_SOURCE             /* See feature_test_macros(7) */

/* pipe(), fork(), dup2(), close(), execlp(), read(), write() precisam das seguintes libs */
#include <unistd.h>

/* pipe() precisa das seguintes libs */
#include <fcntl.h>              /* Obtain O_* constant definitions */
#include <unistd.h>

#include <string.h>
#include <stdlib.h>

char *calculaSha1Sum(char* nomeFicheiro);

char *genericExec3(char* nomeFicheiro);

char *getFileName(char *file);

char *getFileAbsolutePath(char *file);

char *getPermitions(char *ficheiro);

int makeDirectory(char *dirname);

int linker(char *arg1, char *arg2);
