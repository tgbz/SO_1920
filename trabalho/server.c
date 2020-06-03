#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

void interpretMessage(char* message){
    printf("%s",message);
}

int main(int argc, char const *argv[])
{
    int fd;
    int lineChars;
    char buffer[BUFFER_SIZE];

    if (mkfifo("fifo", 0666) == -1)
    {
        perror("mkfifo error");
    }

    if ((fd = open("fifo", O_RDWR)) == -1)
    {
        perror("Erro ao abrir fifo");
        return -1;
    }
    else
    {
        printf("Fifo aberto para escrita!\n");
        while (1)
        {
            lineChars = read(fd, buffer, BUFFER_SIZE);
            buffer[lineChars] = '\0';
            interpretMessage(buffer);
        }
    }

    return 0;
}
