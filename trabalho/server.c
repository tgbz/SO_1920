#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>

#define BUFFER_SIZE 1024

typedef struct instructions
{
    int n;
    int estado; //[0- concluido, 1-max Inativo, 2-max Exec, 3-execução]
    char *task;
    time_t creationTime;
    pid_t pid;
    //char *output;
    struct instructions *next;
}* Instructions;

/* constructInstructions(struct instructions a, char* pipeNumber, char* command, char* task, int estado, int tempoInactivo){
    a->pipeNumber = pipeNumber;
    a->com
} */

/* void getTheData(char* message){
    struct instructions* new = (struct instructions*) malloc(sizeof(struct instructions));
    int i;
    int typeOp = 1;
    char* command;
    char* pipeNumber;
    char* task;
    for (i = 0; message[i]; i++){
        if(message[i]!='#' && typeOp == 1){
            command[i] = message[i];
        }
        else if(message[i]=='#' && typeOp == 1){
            typeOp++;
        }
        else if(message[i]!='#' && typeOp == 2){
            task[i] = message[i];
        }
        else if(message[i]=='#' && typeOp == 2){
            typeOp++;
        }
        else if(message[i]!='#' && typeOp == 3){
            pipeNumber[i] = message[i];
        }
        else if(message[i]=='#' && typeOp == 3){
            typeOp++;
        }
       printf("%s\n%s\n%s", command, pipeNumber, task);
    }
}
 */

Instructions addTarefa(Instructions tarefas, char* task) {
    Instructions newTarefa = malloc(sizeof(struct instructions));
	newTarefa->n = (tarefas==NULL) ? 1 : (tarefas->n)+1;
    newTarefa->estado = 3;
    time(&newTarefa->creationTime);
    newTarefa->task = strdup(task);
    newTarefa->next = tarefas;

    return newTarefa;
}

void printInstructions(Instructions tarefas) {
    if(tarefas!=NULL) {
        printf("Numero: %d   ",tarefas->n);
        printf("Estado: %d   ",tarefas->estado);
        printf("Task: %s   ",tarefas->task);
        printf("PID: %d\n",tarefas->pid);
        printInstructions(tarefas->next);
    }
    else {
        printf("Fim da Lista\n");
    }
}

Instructions changeTarefaState(Instructions tarefas, int nTarefa, int estado) {
    if(tarefas!=NULL)
        if (tarefas->n != nTarefa )
            tarefas->next = changeTarefaState(tarefas->next, nTarefa, estado);
        else if(tarefas->n == nTarefa)
            tarefas->estado = estado;
    return tarefas;
}

void makeList(Instructions tarefas, int fd) {
    if(tarefas!=NULL) {
        if(tarefas->estado==3) {
            char task[1024];
            sprintf(task,"#%d: %s\n",tarefas->n,tarefas->task);
            write(fd, task, strlen(task));
        }
        makeList(tarefas->next,fd);
    }
    else write(fd,"Fim do comando Listar\n", 22);
}

void makeHistorico(Instructions tarefas, int fd) {
    if(tarefas!=NULL) {
        char task[1024];
        if(tarefas->estado==0) sprintf(task,"#%d, concluida: %s\n",tarefas->n,tarefas->task);
        else if(tarefas->estado==1) sprintf(task, "#%d, max inactividade: %s\n",tarefas->n,tarefas->task);
        else if(tarefas->estado==2) sprintf(task, "#%d, max execução: %s\n",tarefas->n,tarefas->task);
        
        if(tarefas->estado!=3) write(fd, task, strlen(task));
        makeHistorico(tarefas->next,fd);
    }
    else write(fd,"Fim do comando Historico\n", 22);
}

 

int exec_command(char *command)
{
    char *exec_args[20];
    char *string;
    int ret = 0;
    int i = 0;

    string = strtok(command, " ");
    while (string != NULL)
    {
        exec_args[i] = string;
        string = strtok(NULL, " ");
        i++;
    }
    exec_args[i] = NULL;

    ret = execvp(exec_args[0], exec_args);

    return ret;
}

int exec_pipes(char *cmds)
{
    char *exec_args[1024];
    char *string;
    int nCmd = 0;

    string = strtok(cmds, "|");
    while (string != NULL)
    {
        exec_args[nCmd] = string;
        string = strtok(NULL, "|");
        nCmd++;
    }
    exec_args[nCmd] = NULL;

    int pipes[nCmd - 1][2];
    int status[nCmd];

    for (int i = 0; i < nCmd; i++)
    {
        if (i == 0)
        {
            if (pipe(pipes[i]) != 0)
            {
                perror("Pipe não foi criado!");
                return -1;
            }
            switch (fork())
            {
            case -1:
                perror("Fork não foi efetuado!");
                return -1;
            case 0:
                // Código Primeiro Filho
                close(pipes[i][0]);
                dup2(pipes[i][1], 1); // ligar stdout do primeiro comando ao extremo de escrita do primeiro pipe
                close(pipes[i][1]);
                exec_command(exec_args[i]);
                _exit(0);
            default:
                // Código Pai
                close(pipes[i][1]);
            }
        }
        else if (i == nCmd - 1)
        {
            switch (fork())
            {
            case -1:
                perror("Fork não foi efetuado!");
                return -1;
            case 0:
                // Código Último Filho
                dup2(pipes[i - 1][0], 0);
                close(pipes[i - 1][0]);
                exec_command(exec_args[i]);
                _exit(0);
            default:
                // Código Pai
                close(pipes[i - 1][0]);
            }
        }
        else
        {
            if (pipe(pipes[i]) != 0)
            {
                perror("Pipe não foi criado!");
                return -1;
            }
            switch (fork())
            {
            case -1:
                perror("Fork não foi efetuado!");
                return -1;
            case 0:
                // Código Filhos do Meio
                close(pipes[i][0]);
                dup2(pipes[i][1], 1); // escreve para o pipe i
                close(pipes[i][1]);
                dup2(pipes[i - 1][0], 0); // lê do pipe i-1
                close(pipes[i - 1][0]);
                exec_command(exec_args[i]);
                _exit(0);
            default:
                // Código Pai
                close(pipes[i][1]);
                close(pipes[i - 1][0]);
            }
        }
    }
    for (int i = 0; i < nCmd; i++)
    {
        wait(&status[i]);
    }
}

void interpretMessage(Instructions *tarefas, char *message)
{
    char *array[4];
    int i = 0;
    int p[2];
    pipe(p);
    int fd;

    char *string;
    string = strtok(message, "#\n");
    while (string != NULL)
    {
        array[i] = string;
        string = strtok(NULL, "#\n");
        i++;
    }
    array[i] = NULL;

    if(strcmp(array[0], "executar") == 0) {
        *tarefas = addTarefa(*tarefas, array[1]);
    }
    /*if(strcmp(array[0], "historico") == 0) {
        *tarefas = changeTarefaState(*tarefas, 1, 0);
        *tarefas = changeTarefaState(*tarefas, 2, 1);
        *tarefas = changeTarefaState(*tarefas, 3, 2);
    }*/
    
    pid_t son = fork();
    if(son==0) {
        if (strcmp(array[0], "executar") == 0)
        {
            char pipeName[1024];
            sprintf(pipeName, "/tmp/%s", array[2]);
            if ((fd = open(pipeName, O_WRONLY)) == -1)
            {
                perror("Erro ao abrir pipe com nome");
                return;
            }
            else
            {
                int n = (*tarefas)->n;
                char stringTarefa[1024];
                sprintf(stringTarefa, "nova tarefa #%d\n", n);
                write(fd, stringTarefa, strlen(stringTarefa));
                exec_pipes(array[1]);
                //Desligar o stdout do servidor;
                /*int saved_stdout;
                saved_stdout = dup(1);
                dup2(fd, 1);
                close(fd);
                dup2(saved_stdout, 1);
                close(saved_stdout);*/
            }
            //printf("%s %s %s\n",array[0],array[1],array[2]);
        }
        else if (strcmp(array[0],"listar") == 0) {
            char pipeName[1024];
            sprintf(pipeName, "/tmp/%s", array[1]);
            if ((fd = open(pipeName, O_WRONLY)) == -1)
            {
                perror("Erro ao abrir pipe com nome");
                return;
            }
            else
            {
                makeList(*tarefas,fd);
                close(fd);
            }   
        }
        else if (strcmp(array[0],"historico") == 0) {
            char pipeName[1024];
            sprintf(pipeName, "/tmp/%s", array[1]);
            if ((fd = open(pipeName, O_WRONLY)) == -1)
            {
                perror("Erro ao abrir pipe com nome");
                return;
            }
            else
            {
                makeHistorico(*tarefas,fd);
                close(fd);
            }   
        }
        else
            printf("%s %s\n", array[0], array[1]);
        _exit(0);
    }
    else 
        (*tarefas)->pid = son;
}

int main(int argc, char const *argv[])
{
    int fd;
    int lineChars;
    char buffer[BUFFER_SIZE];
    Instructions tarefas = NULL;


    //Criar fork com loop em background para verificar tarefas

    if (mkfifo("/tmp/fifo", 0666) == -1)
    {
        perror("mkfifo error");
    }

    while (1)
    {
        if ((fd = open("/tmp/fifo", O_RDWR)) == -1)
        {
            perror("Erro ao abrir fifo");
            return -1;
        }
        else
        {
            printf("Fifo aberto para escrita!\n");
            lineChars = read(fd, buffer, BUFFER_SIZE);
            buffer[lineChars] = '\0';
            printf("%s", buffer);
            interpretMessage(&tarefas, buffer);

            printInstructions(tarefas);
        }
        close(fd);
    }

    return 0;
}
