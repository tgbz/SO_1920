#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>

#define BUFFER_SIZE 1024
#define SigConclude 34
#define SigMaxExec 35
#define SigMaxInac 36
#define SigTerm 37
#define NELEMS(x) (sizeof(x) / sizeof((x)[0]))

void maxInacHandlerSon(int signum);
void maxExecHandlerSon(int signum);
void maxInacHandlerFather(int signum);
void concludeInstructionsHandler(int signum);
void terminarHandler(int signum);

typedef struct instructions
{
    int n;
    int estado; //[0- concluido, 1-max Inativo, 2-max Exec, 3-terminada pelo utilizador, 4-execução]
    char *task;
    time_t creationTime;
    pid_t pid;
    //char *output;
    struct instructions *next;
} * Instructions;

Instructions tarefas;
int maxExec = 0;
int maxInac = 0;
int pidPipeConclude[2];
int pidPipeAlarm[2];
pid_t pidSon=0;
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
    newTarefa->estado = 4;
    time(&newTarefa->creationTime);
    newTarefa->task = strdup(task);
    newTarefa->next = tarefas;

    return newTarefa;
}

void printInstructions(Instructions tarefas)
{
    if (tarefas != NULL)
    {
        fprintf(stderr, "|\tNumero: %d   ", tarefas->n);
        fprintf(stderr, "Estado: %d   ", tarefas->estado);
        fprintf(stderr, "Task: %s   ", tarefas->task);
        fprintf(stderr, "PID: %d\n", tarefas->pid);
        printInstructions(tarefas->next);
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
        else if (tarefas->estado == 3)
            sprintf(task, "#%d, terminado: %s\n", tarefas->n, tarefas->task);

        if (tarefas->estado != 4)
            write(fd, task, strlen(task));
        makeHistorico(tarefas->next, fd);
    }
    else
        write(fd, "Fim do comando Historico\n", 25);
}

int exec_command(char *command, int pipes[][2], int i, int nCmd)
{
    if (i == 0)
    {
        close(pipes[i][0]);
        dup2(pipes[i][1], 1);
        close(pipes[i][1]);
    }
    else if (i == nCmd - 1)
    {
        dup2(pipes[i - 1][0], 0);
        close(pipes[i - 1][0]);
    }
    else
    {
        close(pipes[i][0]);
        dup2(pipes[i][1], 1);
        close(pipes[i][1]);
        dup2(pipes[i - 1][0], 0);
        close(pipes[i - 1][0]);
    }
    char *exec_args[20];
    char *string;
    int ret = 0;
    int j = 0;

    string = strtok(command, " ");
    while (string != NULL)
    {
        exec_args[j] = string;
        string = strtok(NULL, " ");
        j++;
    }
    exec_args[j] = NULL;

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
    //int status[nCmd];
    pid_t son;

    for (int i = 0; i < nCmd; i++)
    {
        int status;
        pipe(pipes[i]);
        son = fork();
        pidSon = son;
        if (son == 0)
        {
            signal(SigTerm, terminarHandler);
            signal(SIGALRM, maxInacHandlerSon);
            if (maxInac)
                alarm(maxInac);
            son = fork();
            pidSon = son;
            if (son == 0){  
                signal(SigTerm, terminarHandler);
                exec_command(exec_args[i], pipes, i, nCmd);
            }
            else
            {
                if (i == 0)
                {
                    // Código Pai
                    close(pipes[i][1]);
                }
                else if (i == nCmd - 1)
                {
                    // Código Pai
                    close(pipes[i - 1][0]);
                }
                else
                {
                    // Código Pai
                    close(pipes[i][1]);
                    close(pipes[i - 1][0]);
                }
            }
            wait(NULL);
            _exit(0);
        }

        wait(&status);
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
    {
        *tarefas = addTarefa(*tarefas, array[1]);
        pid_t son = fork();
        pidSon = son;
        if (son == 0)
        {
            signal(SIGALRM, maxExecHandlerSon);
            signal(SigMaxInac, maxInacHandlerFather);
            signal(SigTerm, terminarHandler);
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

                if (maxExec)
                    alarm(maxExec);
                sprintf(pipeNumb, "%d\n", getpid());

                exec_pipes(array[1]);

                write(pidPipeConclude[1], pipeNumb, strlen(pipeNumb));
                kill(getppid(), SigConclude);
            }
            //fprintf(stderr,"%s %s %s\n",array[0],array[1],array[2]);
            _exit(0);
        }
        else
        {
            (*tarefas)->pid = son;
        }
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
    else if (strcmp(array[0], "tempo-inactividade") == 0)
    {
        maxInac = atoi(array[1]);
    }
    else if (strcmp(array[0], "tempo-execucao") == 0)
    {
        maxExec = atoi(array[1]);
    }
    else if (strcmp(array[0], "terminar") == 0)
    {
        int nTarefa = atoi(array[1]);
        Instructions aux = *tarefas;
        while (aux != NULL)
            {
                if (aux->n == nTarefa)
                {
                    aux->estado = 3;
                    kill(aux->pid, SigTerm);
                    break;
                }
                else{
                    aux = aux->next;}
            }
    }
}

void concludeInstructionsHandler(int signum)
{
    fprintf(stderr, "concludeInstructionsHandler\n");
    if (signum == SigConclude)
    {
        char buffer[BUFFER_SIZE];

        if (readln(pidPipeConclude[0], buffer, BUFFER_SIZE))
        {
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
    fprintf(stderr, "maxExecHandlerFather\n");
    if (signum == SigMaxExec)
    {
        char buffer[BUFFER_SIZE];
        if (readln(pidPipeAlarm[0], buffer, BUFFER_SIZE))
        {
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
    fprintf(stderr, "maxExecHandlerSon\n");
    // mata filhos
    // manda mensagem no pipe para pai e signal para ler pipe e mudar estado
    // SUICIDIO
    if (signum == SIGALRM)
    {
        write(pidPipeAlarm[1], pipeNumb, strlen(pipeNumb));
        kill(getppid(), SigMaxExec);
        kill(pidSon, SIGKILL);
        kill(getpid(), SIGKILL);
    }
}

void maxInacHandlerGrandFather(int signum)
{
    fprintf(stderr, "maxInacHandlerGrandFather\n");
    if (signum == SigMaxInac)
    {
        char buffer[BUFFER_SIZE];
        if (readln(pidPipeAlarm[0], buffer, BUFFER_SIZE))
        {
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
    fprintf(stderr, "maxInacHandlerFather\n");
    if (signum == SigMaxInac)
    {
        write(pidPipeAlarm[1], pipeNumb, strlen(pipeNumb));
        kill(getppid(), SigMaxInac);
        kill(pidSon, SIGKILL);
        kill(getpid(), SIGKILL);
    }
}

void maxInacHandlerSon(int signum)
{
    fprintf(stderr, "maxInacHandlerSon\n");
    if (signum == SIGALRM)
        kill(getppid(), SigMaxInac);
}


void terminarHandler(int signum)
{
    fprintf(stderr, "terminarHandler\n");
    if (signum == SigTerm)
    {
        int pid = getpid();
        if ((pidSon!=0) && (pid!=pidSon))
            kill(pidSon, SigTerm);
        kill(pid, SIGKILL);
    }
}

int main(int argc, char const *argv[])
{
    signal(SigConclude, concludeInstructionsHandler);
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
            fprintf(stderr, "Fifo aberto para leitura!\n");
            lineChars = read(fd, buffer, BUFFER_SIZE);
            buffer[lineChars] = '\0';
            fprintf(stderr, "%s", buffer);
            interpretMessage(&tarefas, buffer);
            printInstructions(tarefas);
        }
        close(fd);
    }

    return 0;
}
