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
#define SigConclude 34
#define SigMaxExec 35
#define SigMaxInac 36
#define NELEMS(x) (sizeof(x) / sizeof((x)[0]))

void maxExecHandlerSon(int signum);
void maxInacHandlerSon(int signum);

typedef struct instructions
{
    int n;
    int estado; //[0- concluido, 1-max Inativo, 2-max Exec, 3-execução]
    char *task;
    time_t creationTime;
    pid_t pid;
    //char *output;
    struct instructions *next;
} * Instructions;

Instructions tarefas;
int maxExec = 18;
int maxInac = 3;
pid_t pidGlobal;
int pidPipeConclude[2];
int pidPipeAlarm[2];
pid_t pidGrandSon;
char pipeNumb[1024];

ssize_t readln(int fildes, void *buffer, ssize_t numBytes)
{

    ssize_t res = 0;
    int i = 0;

    while (i < numBytes && (res = read(fildes, &buffer[i], 1)) > 0)
    {

        if (((char *)buffer)[i] == '\n')
            return (i + 1);
        i += res;
    }

    return i;
}

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

void clearBuf(char *b)
{
    int i;
    for (i = 0; i < BUFFER_SIZE; i++)
        b[i] = '\0';
}

Instructions addTarefa(Instructions tarefas, char *task)
{
    Instructions newTarefa = malloc(sizeof(struct instructions));
    newTarefa->n = (tarefas == NULL) ? 1 : (tarefas->n) + 1;
    newTarefa->estado = 3;
    time(&newTarefa->creationTime);
    newTarefa->task = strdup(task);
    newTarefa->next = tarefas;

    return newTarefa;
}

void printInstructions(Instructions tarefas)
{
    if (tarefas != NULL)
    {
        fprintf(stderr, "Numero: %d   ", tarefas->n);
        fprintf(stderr, "Estado: %d   ", tarefas->estado);
        fprintf(stderr, "Task: %s   ", tarefas->task);
        fprintf(stderr, "PID: %d\n", tarefas->pid);
        printInstructions(tarefas->next);
    }
    else
    {
        fprintf(stderr, "Fim da Lista\n");
    }
}

Instructions changeTarefaState(Instructions tarefas, int nTarefa, int estado)
{
    if (tarefas != NULL)
        if (tarefas->n != nTarefa)
            tarefas->next = changeTarefaState(tarefas->next, nTarefa, estado);
        else if (tarefas->n == nTarefa)
            tarefas->estado = estado;
    return tarefas;
}

void makeList(Instructions tarefas, int fd)
{
    if (tarefas != NULL)
    {
        if (tarefas->estado == 3)
        {
            char task[1024];
            sprintf(task, "#%d: %s\n", tarefas->n, tarefas->task);
            write(fd, task, strlen(task));
        }
        makeList(tarefas->next, fd);
    }
    else
        write(fd, "Fim do comando Listar\n", 22);
}

void makeHistorico(Instructions tarefas, int fd)
{
    if (tarefas != NULL)
    {
        char task[1024];
        if (tarefas->estado == 0)
            sprintf(task, "#%d, concluida: %s\n", tarefas->n, tarefas->task);
        else if (tarefas->estado == 1)
            sprintf(task, "#%d, max inactividade: %s\n", tarefas->n, tarefas->task);
        else if (tarefas->estado == 2)
            sprintf(task, "#%d, max execução: %s\n", tarefas->n, tarefas->task);

        if (tarefas->estado != 3)
            write(fd, task, strlen(task));
        makeHistorico(tarefas->next, fd);
    }
    else
        write(fd, "Fim do comando Historico\n", 25);
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
    int son;
    for (int i = 0; i < nCmd; i++)
    {
        if (i == 0)
        {
            if (pipe(pipes[i]) != 0)
            {
                perror("Pipe não foi criado!");
                return -1;
            }
            switch (son = fork())
            {
            case -1:
                perror("Fork não foi efetuado!");
                return -1;
            case 0:
                signal(SIGALRM, maxInacHandlerSon);
                alarm(maxInac);
                // Código Primeiro Filho
                close(pipes[i][0]);
                dup2(pipes[i][1], 1); // ligar stdout do primeiro comando ao extremo de escrita do primeiro pipe
                close(pipes[i][1]);
                exec_command(exec_args[i]);
                fprintf(stderr,"1");
                _exit(0);
            default:
                // Código Pai
                pidGrandSon = son;
                close(pipes[i][1]);
                fprintf(stderr,"2");
                wait(&status[i]);
                fprintf(stderr,"3");
            }
        }
        else if (i == nCmd - 1)
        {
            switch (son = fork())
            {
            case -1:
                perror("Fork não foi efetuado!");
                return -1;
            case 0:
                signal(SIGALRM, maxInacHandlerSon);
                alarm(maxInac);
                // Código Último Filho
                dup2(pipes[i - 1][0], 0);
                close(pipes[i - 1][0]);
                exec_command(exec_args[i]);
                _exit(0);
            default:
                // Código Pai
                pidGrandSon = son;
                close(pipes[i - 1][0]);
                wait(&status[i]);
            }
        }
        else
        {
            if (pipe(pipes[i]) != 0)
            {
                perror("Pipe não foi criado!");
                return -1;
            }
            switch (son = fork())
            {
            case -1:
                perror("Fork não foi efetuado!");
                return -1;
            case 0:
                fprintf(stderr,"4");
                signal(SIGALRM, maxInacHandlerSon);
                alarm(maxInac);
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
                pidGrandSon = son;
                close(pipes[i][1]);
                close(pipes[i - 1][0]);
                wait(&status[i]);
            }
        }
    }
}


void interpretMessage(Instructions *tarefas, char *message)
{
    char *array[4];
    int i = 0;
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

    if (strcmp(array[0], "executar") == 0)
        *tarefas = addTarefa(*tarefas, array[1]);

    pid_t son = fork();
    pidGlobal = son;
    if (son == 0)
    {
        signal(SIGALRM, maxExecHandlerSon);
        signal(SigMaxInac, maxInacHandlerSon);
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
                close(fd);

                alarm(maxExec);
                sprintf(pipeNumb, "%d\n", getpid());

                exec_pipes(array[1]);

                write(pidPipeConclude[1], pipeNumb, strlen(pipeNumb));
                kill(getppid(), SigConclude);
            }
            //fprintf(stderr,"%s %s %s\n",array[0],array[1],array[2]);
        }
        else if (strcmp(array[0], "listar") == 0)
        {
            char pipeName[1024];
            sprintf(pipeName, "/tmp/%s", array[1]);
            if ((fd = open(pipeName, O_WRONLY)) == -1)
            {
                perror("Erro ao abrir pipe com nome");
                return;
            }
            else
            {
                makeList(*tarefas, fd);
                close(fd);
            }
        }
        else if (strcmp(array[0], "historico") == 0)
        {
            char pipeName[1024];
            sprintf(pipeName, "/tmp/%s", array[1]);
            if ((fd = open(pipeName, O_WRONLY)) == -1)
            {
                perror("Erro ao abrir pipe com nome");
                return;
            }
            else
            {
                makeHistorico(*tarefas, fd);
                close(fd);
            }
        }
        else
            fprintf(stderr, "%s %s\n", array[0], array[1]);
        _exit(0);
    }
    else
    {
        if (strcmp(array[0], "executar") == 0)
        {
            (*tarefas)->pid = son;
            //concludeInstructions();
        }
    }
}

void concludeInstructions(int signum)
{
    if (signum == SigConclude)
    {
        char buffer[BUFFER_SIZE];

        if (readln(pidPipeConclude[0], buffer, BUFFER_SIZE))
        {
            fprintf(stderr, "PID: %s\n", buffer);
            Instructions aux = tarefas;
            while (aux != NULL)
            {
                if (aux->pid == atoi(buffer))
                {
                    aux->estado = 0;
                    kill(aux->pid, SIGKILL);
                    break;
                }
                else
                    aux = aux->next;
            }
        }
    }
}

void maxExecHandlerFather(int signum)
{
    if (signum == SigMaxExec)
    {
        char buffer[BUFFER_SIZE];
        if (readln(pidPipeAlarm[0], buffer, BUFFER_SIZE))
        {
            fprintf(stderr,"MaxExec %s\n", buffer);
            Instructions aux = tarefas;
            while (aux != NULL)
            {
                if (aux->pid == atoi(buffer))
                {
                    aux->estado = 2;
                    break;
                }
                else
                    aux = aux->next;
            }
        }
    }
}

void maxExecHandlerSon(int signum)
{
    // mata filhos
    // manda mensagem no pipe para pai e signal para ler pipe e mudar estado
    // SUICIDIO
    if (signum == SIGALRM)
    {
        write(pidPipeAlarm[1], pipeNumb, strlen(pipeNumb));
        kill(getppid(), SigMaxExec);
        kill(pidGrandSon, SIGKILL);
        kill(getpid(), SIGKILL);
    }
}

void maxInacHandlerGrandFather(int signum)
{
    if (signum == SigMaxInac)
    {
        char buffer[BUFFER_SIZE];
        if (readln(pidPipeAlarm[0], buffer, BUFFER_SIZE))
        {
            fprintf(stderr,"MaxInac %s\n", buffer);
            Instructions aux = tarefas;
            while (aux != NULL)
            {
                if (aux->pid == atoi(buffer))
                {
                    aux->estado = 1;
                    break;
                }
                else
                    aux = aux->next;
            }
        }
    }
}

void maxInacHandlerFather(int signum)
{
    if (signum == SigMaxInac)
    {
        write(pidPipeAlarm[1], pipeNumb, strlen(pipeNumb));
        kill(getppid(), SigMaxInac);
        kill(pidGrandSon, SIGKILL);
        kill(getpid(), SIGKILL);
    }
}

void maxInacHandlerSon(int signum)
{
    fprintf(stderr, "\nteste\n");
    if (signum == SIGALRM)
        kill(getppid(), SigMaxInac);
}

int main(int argc, char const *argv[])
{
    signal(SigConclude, concludeInstructions);
    signal(SigMaxExec, maxExecHandlerFather);
    signal(SigMaxInac, maxInacHandlerGrandFather);
    pipe(pidPipeConclude);
    pipe(pidPipeAlarm);
    int fd;
    int lineChars;
    char buffer[BUFFER_SIZE];
    tarefas = NULL;

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
            fprintf(stderr, "Fifo aberto para escrita!\n");
            lineChars = read(fd, buffer, BUFFER_SIZE);
            buffer[lineChars] = '\0';
            fprintf(stderr, "%s", buffer);
            interpretMessage(&tarefas, buffer);
            //concludeInstructions(pidPipeConclude);
            printInstructions(tarefas);
        }
        close(fd);
    }

    return 0;
}
