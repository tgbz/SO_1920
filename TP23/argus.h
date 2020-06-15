#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>



#define BUFFER_SIZE 1024
#define MAX_PIPES 1024
#define MAX_LINE_SIZE 1024
#define FIFO_NAME "fifo"