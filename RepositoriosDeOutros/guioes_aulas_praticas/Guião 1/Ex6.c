#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>   
#include <fcntl.h>


#define SIZE 200
#define FILENAME "pessoas"


typedef struct Person{
    char name[200];
    int age;
} Person;


// create a new person
int newPerson (char* name, int age){

    Person p;
    strcpy(p.name, name);
    p.age = age;

    int fd = open(FILENAME, O_CREAT | O_APPEND | O_WRONLY, 0600);

    int res = write(fd, &p, sizeof(Person));
    if (res < 0){
        printf("Error write\n");
        return -1;
    }

    printf("%s %d created\n", p.name, p.age);
    close(fd);
    return 0;
}


// change a person's age
int changeAge (char* name, int age){

    Person p;

    int fd = open(FILENAME, O_RDWR, 0600);
    if (fd < 0){
        printf("Error open\n");
        return -1;
    }

    int bytes_read;

    while ((bytes_read = read(fd, &p, sizeof(Person))) > 0){

        int res;

        if (strcmp(p.name, name) == 0){
            p.age = age;

            res = lseek(fd, -sizeof(Person), SEEK_CUR);
            if (res < 0){
                printf("Error lseek\n");
                return -1;
            }
        }

        res = write(fd, &p, sizeof(Person));
        if (res < 0){
            printf("Error write\n");
            return -1;
        }
    }

    printf("%s %d edited\n", p.name, p.age);
    close(fd);
    return 0;
}


int main (int argc, char* argv[]){

    if (strcmp(argv[1], "-i") == 0) newPerson (argv[2], atoi(argv[3]));
    if (strcmp(argv[1], "-u") == 0) changeAge (argv[2], atoi(argv[3]));
    return 0;
}

/*
Testes:
./a.out -i Mourinho 55
./a.out -u Mourinho 56
*/