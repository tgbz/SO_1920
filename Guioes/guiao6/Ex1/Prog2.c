#include <stdio.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> 

#define MAX_LINE_SIZE 1024

int main(int argc, char const *argv[])
{
    int fd;
    int bytesRead;
    char buffer[MAX_LINE_SIZE];

    if ((fd=open("fifo", O_WRONLY))==-1){
        perror("open");
        return -1;
    }
    else printf("Opened fifo for writing \n");
    
    while((bytesRead = read(0,&buffer,MAX_LINE_SIZE))>0){
        write(fd,&buffer,bytesRead);
    }
    if(bytesRead==0){
        printf("EOF\n");
    }
    else perror("read");
    close(fd);
    return 0;
}
