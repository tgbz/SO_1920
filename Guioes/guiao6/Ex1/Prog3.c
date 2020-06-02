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

    if ((fd=open("fifo",O_RDONLY))==-1){
        perror("open");
        return -1;
    }
    else printf("Opened fifo for reading\n");

    while((bytesRead = read(fd,&buffer,MAX_LINE_SIZE))>0){
        write(1,&buffer,bytesRead);
    }
    if(bytesRead==0){
        printf("EOF\n");
    }
    else perror("read");

    close(fd);
    unlink("fifo");
    return 0;
}
