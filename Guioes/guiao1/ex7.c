#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<sys/types.h>
#include<unistd.h>
#include<fcntl.h>

#define SIZE 200
#define FILENAME "pessoas"

//definição da estrutura Pessoa. 
typedef struct Person {
    char name[200];
    int age;
} Person;


//criar nova pessoa
int newPerson (char* name, int age){
    Person p;
    strcpy(p.name, name);
    p.age = age;

    int fd = open(FILENAME, O_CREAT | O_APPEND | O_WRONLY, 0600);

    int res = write (fd, &p, sizeof(Person));
    if(res == -1){
        printf("Erro a criar pessoa! \n");
        return -1;
    }

    int lseek_res = lseek(fd, -sizeof(Person), SEEK_CUR);
    if (lseek_res < 0){
        printf("Error lseek\n");
        return -1;
    }
    close(fd);
    return lseek_res;
}

//Alterar a idade de uma pessoa
int changeAge (int pos, int age){

    Person p;

    int fd = open(FILENAME, O_RDWR, 0600);

    int seek_res = lseek(fd, pos, SEEK_SET);
    if (seek_res < 0){
        printf("Error lseek\n");
        return -1;
    }

    int bytes_read = read(fd, &p, sizeof(Person));
    p.age = age;

    seek_res = lseek(fd, -sizeof(Person), SEEK_CUR);
    int res = write(fd, &p, sizeof(Person));
    if (res < 0){
        printf("Error write \n");
        return -1;
    }

    printf("%s, %d edited\n", p.name, p.age);
    close(fd);
    return 0;
}


int main(int argc, char const *argv[])
{
    if(strcmp(argv[1], "-i") == 0){
        int registo = newPerson (argv[2],atoi(argv[3]));
        printf("registo %d\n", registo);
    }
    if (strcmp(argv[1], "-u") == 0){
        changeAge (atoi(argv[2]), atoi(argv[3]));
    }
    return 0;
}

