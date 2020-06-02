#include <stdio.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> 

#define MAX_LINE_SIZE 1024

int main(int argc, char const *argv[])
{
    if(mkfifo("fifo",0666)==-1){
        perror("mkfifo");
    }
    
    int fd;
    int bytesRead;
    char buffer[MAX_LINE_SIZE];
    int logsFile;

    if((logsFile=open("logs.txt",O_CREAT | O_TRUNC | O_WRONLY, 0666))==-1){
        perror("open");
        return -1;
    }

    while(1){
        if((fd=open("fifo",O_RDONLY))==-1){
            perror("open");
            return -1;
        }
        while((bytesRead=read(fd,&buffer,MAX_LINE_SIZE))>0){
            write(logsFile,&buffer,bytesRead);
        }
        if(bytesRead==0){
            printf("EOF\n");
        }
        else {
            perror("read");
    }
    close(fd);
    }
    //Executado em caso de erro
    close(fd);
    close(logsFile);

    return 0;
}
