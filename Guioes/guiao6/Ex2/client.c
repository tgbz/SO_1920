#include <stdio.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> 

#define MAX_LINE_SIZE 1024

int main(int argc, char const *argv[])
{
    int fd = open("fifo",O_WRONLY);

    if(fd == -1){
        perror("open");
        return -1;
    }
    for (int i = 1; i<argc;i++){
        write(fd,argv[i],strlen(argv[i]));
        write(fd,"\n",1);
    }
    close(fd);
    return 0;
}
