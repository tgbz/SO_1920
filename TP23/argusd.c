#include"argus.h"
#include <sys/wait.h>
#include <signal.h>


#define SigConclude 34
#define SigMaxExec 35
#define SigMaxInac 36
#define SigTerm 37

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
    pid_t pid;
    struct instructions *next;
} * Instructions;

Instructions tarefas;
int maxExec = 0;
int maxInac = 0;
int pidPipeConclude[2];
int pidPipeAlarm[2];
pid_t pidSon = 0;
char pipeNumb[1024];

//Função de leitura por linha
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

//Adicona uma tarefa há estrutura existente
Instructions addTarefa(Instructions tarefas, int nTarefa, char *task, int estado)
{
    Instructions newTarefa = malloc(sizeof(struct instructions));
    newTarefa->n = nTarefa;
    newTarefa->estado = estado;
    newTarefa->task = strdup(task);
    newTarefa->next = tarefas;

    return newTarefa;
}

//Print da estrutura de tarefas (Debug)
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

//Imprime para o ficheiro (fd) a lista de tarefas em execução
void makeList(Instructions tarefas, int fd)
{
    if (tarefas != NULL)
    {
        if (tarefas->estado == 4)
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

//Imprime para o pipe do cliente (fd) o output da tarefa com numero ntarefa
void makeOutput(int ntarefa, int fd)
{
    int logIDX = open("log.idx", O_RDWR | O_CREAT | O_APPEND, 0666);
    char buffer[BUFFER_SIZE];
    //Le linhas do ficheiro com formato (numero, codigo, possição de inicio, numero de caracteres do output), separados por #
    while (readln(logIDX, buffer, BUFFER_SIZE))
    {

        char *args[4];
        char *string;
        int j = 0;
        //Separa a string lida do ficheiro log.idx por #
        string = strtok(buffer, "#");
        while (string != NULL)
        {
            args[j++] = string;
            string = strtok(NULL, "#");
        }

        if (atoi(args[0]) == ntarefa)
        {
            int charInit = atoi(args[2]);
            int size = atoi(args[3]);
            int logFile = open("log", O_RDWR | O_CREAT | O_APPEND, 0666);
            //Define o inicio de leitura em função da possição inicial lida
            lseek(logFile, charInit, SEEK_SET);
            char buffer[size];
            read(logFile, buffer, size);
            //escreve no cliente (fd) o output
            write(fd, buffer, size);
            close(logIDX);
            close(logFile);
            return;
        }
    }
    write(fd, "Tarefa nao encontrada\n", 22);
}

//Imprime para o ficheiro (fd) o historico das tarefas  
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

//Executa o comando lido (sem pipes)
int exec_command(char *command)
{
    char *exec_args[MAX_PIPES];
    char *string;
    int ret = 0;
    int i = 0;
    //Separa os args do comando (ex: ls -l => ["ls","-l"])
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

//Trata de separa os comandos recebidos e encaminhar os outputs para os inputs de forma sequencial
//É responsavel por escrever o output do ultimo comando nos ficheiros de log
int exec_pipes(char *cmds)
{
    char *exec_args[1024];
    char *string;
    int nCmd = 0;
    //Separa os comandos por pipes ("|")
    string = strtok(cmds, "|");
    while (string != NULL)
    {
        exec_args[nCmd] = string;
        string = strtok(NULL, "|");
        nCmd++;
    }
    exec_args[nCmd] = NULL;
    //Cria uma matrix de pipes do tamanho de comandos lidos por 2 (input, output)
    int pipes[nCmd - 1][2];
    int son;
    for (int i = 0; i < nCmd; i++)
    {
        //Ultimo filho (escreve nos ficheiros logs)
        if (i == nCmd - 1)
        {
            switch (son = fork())
            {
            case -1:
                perror("Fork nao foi efetuado!");
                return -1;
            case 0:
                //Cria sinais para terminar e executar o max inactividade
                signal(SigTerm, terminarHandler);
                signal(SIGALRM, maxInacHandlerSon);
                //Cria o timer para a max inactividade
                if (maxInac)
                    alarm(maxInac);
                // Código ultimo Filho
                int count = 0;
                int logFile;
                logFile = open("log", O_RDWR | O_CREAT | O_APPEND, 0666);
                char buffer[1024];
                int j = 0;
                //Conta o numero de bytes já existentes no ficheiro logs (para ter a posição inicial do proximo comando)
                while (j = read(logFile, buffer, 1024))
                    count += j;
                dup2(logFile, 1);

                dup2(pipes[i - 1][0], 0);
                close(pipes[i - 1][0]);
                //Executa o comando atual
                if ((pidSon = fork()) == 0)
                {
                    signal(SigTerm, terminarHandler);
                    exec_command(exec_args[i]);
                }
                wait(NULL);
                
                int countF = 0;
                char buffer2[1024];
                j = 0;
                //Reinicia a posição do ficheiro logs
                lseek(logFile, 0, SEEK_SET);
                //Re-le o ficheiro logs para saber o tamanho do resultado do comando executado (countF-count);
                while (j = read(logFile, buffer2, 1024))
                    countF += j;
                int logIDX = open("log.idx", O_RDWR | O_CREAT | O_APPEND, 0666);
                char tarCount[1024];
                sprintf(tarCount, "%d#%s#%d#%d\n", tarefas->n, tarefas->task, count, countF - count);
                //Escreve no ficheiro log.idx 
                write(logIDX, tarCount, strlen(tarCount));
                close(logFile);
                close(logIDX);
                _exit(0);
            default:
                // Código Pai
                pidSon = son;
                close(pipes[i - 1][0]);
            }
        }
        //Primeiro filho
        else if (i == 0)
        {
            if (pipe(pipes[i]) != 0)
            {
                perror("Pipe nao foi criado!");
                return -1;
            }
            switch (son = fork())
            {
            case -1:
                perror("Fork nao foi efetuado!");
                return -1;
            case 0:
                //Cria sinais para terminar e executar o max inactividade
                signal(SigTerm, terminarHandler);
                signal(SIGALRM, maxInacHandlerSon);
                //Cria o timer para a max inactividade
                if (maxInac)
                    alarm(maxInac);

                // Código Primeiro Filho
                close(pipes[i][0]);
                dup2(pipes[i][1], 1); // ligar stdout do primeiro comando ao extremo de escrita do primeiro pipe
                close(pipes[i][1]);
                //Executa o comando atual
                if ((pidSon = fork()) == 0)
                {
                    signal(SigTerm, terminarHandler);
                    exec_command(exec_args[i]);
                }
                wait(NULL);
                _exit(0);
            default:
                // Código Pai
                pidSon = son;
                close(pipes[i][1]);
            }
        }
        else //Comandos intermedios (filhos do meio)
        {
            if (pipe(pipes[i]) != 0)
            {
                perror("Pipe nao foi criado!");
                return -1;
            }
            switch (son = fork())
            {
            case -1:
                perror("Fork nao foi efetuado!");
                return -1;
            case 0:
                //Cria sinais para terminar e executar o max inactividade
                signal(SigTerm, terminarHandler);
                signal(SIGALRM, maxInacHandlerSon);
                //Cria o timer para a max inactividade
                if (maxInac)
                    alarm(maxInac);
                // Código Filhos do Meio
                close(pipes[i][0]);
                dup2(pipes[i][1], 1); // escreve para o pipe i
                close(pipes[i][1]);
                dup2(pipes[i - 1][0], 0);
                close(pipes[i - 1][0]);
                //Executa o comando atual
                if ((pidSon = fork()) == 0)
                {
                    signal(SigTerm, terminarHandler);
                    exec_command(exec_args[i]);
                }
                wait(NULL);
                _exit(0);
            default:
                // Código Pai
                pidSon = son;
                close(pipes[i][1]);
                close(pipes[i - 1][0]);
            }
        }
        wait(NULL);
    }
}

//Parser das instruções do cliente
void interpretMessage(Instructions *tarefas, char *message)
{
    char *array[4];
    int i = 0;
    int fd;
    //Separa a informação do comando (ex: "executar#ls -l#[pid do client]") 
    char *string;
    string = strtok(message, "#\n");
    while (string != NULL)
    {
        array[i] = string;
        string = strtok(NULL, "#\n");
        i++;
    }
    array[i] = NULL;
    //Tarefa executar
    if (strcmp(array[0], "executar") == 0)
    {
        //Define o n da nova tarefa, e adiciona há estrutura
        int nTarefa = (*tarefas == NULL) ? 1 : ((*tarefas)->n) + 1;
        *tarefas = addTarefa(*tarefas, nTarefa, array[1], 4);
        pid_t son = fork();
        pidSon = son;
        if (son == 0)
        {
            //Cria sinais para terminar e executar o max inactividade / execução
            signal(SIGALRM, maxExecHandlerSon);
            signal(SigMaxInac, maxInacHandlerFather);
            signal(SigTerm, terminarHandler);
            char pipeName[1024];
            sprintf(pipeName, "%s", array[2]);
            //Abre o pipe com nome para responder ao cliente
            if ((fd = open(pipeName, O_WRONLY)) == -1)
            {
                perror("Erro ao abrir pipe com nome");
                return;
            }
            else
            {
                int n = (*tarefas)->n;
                //Escreve no cliente que a tarefa foi criada com sucesso
                char stringTarefa[1024];
                sprintf(stringTarefa, "nova tarefa #%d\n", n);
                write(fd, stringTarefa, strlen(stringTarefa));
                close(fd);

                //Cria o timer para a max execucao
                if (maxExec)
                    alarm(maxExec);
                sprintf(pipeNumb, "%d\n", getpid());
                //Inicia a execução do comando recevido
                exec_pipes(array[1]);

                write(pidPipeConclude[1], pipeNumb, strlen(pipeNumb));
                //Envia(kill) ao pai que o comando foi executado com sucesso
                kill(getppid(), SigConclude);
            }
            _exit(0);
        }
        else
        {
            (*tarefas)->pid = son;
        }
    }
    //Tarefa Listar
    else if (strcmp(array[0], "listar") == 0)
    {
        char pipeName[1024];
        sprintf(pipeName, "%s", array[1]);
        //Abre o pipe com nome para responder ao cliente
        if ((fd = open(pipeName, O_WRONLY)) == -1)
        {
            perror("Erro ao abrir pipe com nome");
            return;
        }
        else
        {
            //Imprime a lista de tarefas em execução
            makeList(*tarefas, fd);
            close(fd);
        }
    }
    else if (strcmp(array[0], "historico") == 0)
    {
        char pipeName[1024];
        sprintf(pipeName, "%s", array[1]);
        //Abre o pipe com nome para responder ao cliente
        if ((fd = open(pipeName, O_WRONLY)) == -1)
        {
            perror("Erro ao abrir pipe com nome");
            return;
        }
        else
        {
            //Imprime o historico de tarefas
            makeHistorico(*tarefas, fd);
            close(fd);
        }
    }
    //Tarefa max inatividade
    else if (strcmp(array[0], "tempo-inactividade") == 0)
    {
        maxInac = atoi(array[1]);
    }
    //Tarefa max execução
    else if (strcmp(array[0], "tempo-execucao") == 0)
    {
        maxExec = atoi(array[1]);
    }
    //Tarefa terminar
    else if (strcmp(array[0], "terminar") == 0)
    {
        //Le o numero da tarefa e altera o estado da tarefa na estrutura
        int nTarefa = atoi(array[1]);
        Instructions aux = *tarefas;
        while (aux != NULL)
        {
            if (aux->n == nTarefa)
            {
                if (aux->estado == 4)
                {
                    aux->estado = 3;
                    //Termina a execução do processo que está a executar a tarefa
                    kill(aux->pid, SigTerm);
                }
                break;
            }
            else
            {
                aux = aux->next;
            }
        }
    }
    //Tarefa output
    else if (strcmp(array[0], "output") == 0)
    {
        int nTarefa = atoi(array[1]);
        char pipeName[1024];
        sprintf(pipeName, "%s", array[2]);
        //Abre o pipe com nome para responder ao cliente
        if ((fd = open(pipeName, O_WRONLY)) == -1)
        {
            perror("Erro ao abrir pipe com nome");
            return;
        }
        else
        {
            //Imprime o output da tarefa nTarefas
            makeOutput(nTarefa, fd);
            close(fd);
        }
    }
}

//Handler de concluir instrução
void concludeInstructionsHandler(int signum)
{
    fprintf(stderr,"concludeInstructionsHandler\n");
    if (signum == SigConclude)
    {
        char buffer[BUFFER_SIZE];
        //Le do pipe qual a tarefa a concluir
        if (readln(pidPipeConclude[0], buffer, BUFFER_SIZE))
        {
            //Altera o estado da tarefa
            Instructions aux = tarefas;
            while (aux != NULL)
            {
                if (aux->pid == atoi(buffer))
                {
                    aux->estado = 0;
                    //Mata o processo que a esta a executar
                    kill(aux->pid, SIGKILL);
                    break;
                }
                else
                    aux = aux->next;
            }
        }
    }
}

//Handler de max Execução para processo pai
void maxExecHandlerFather(int signum)
{
    fprintf(stderr,"maxExecHandlerFather\n");
    if (signum == SigMaxExec)
    {
        char buffer[BUFFER_SIZE];
        //Le do pipe qual tarefa a tratar
        if (readln(pidPipeAlarm[0], buffer, BUFFER_SIZE))
        {
            //Altera o estado da tarefa
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

//Handler de max execução para processos filho
void maxExecHandlerSon(int signum)
{
    fprintf(stderr,"maxExecHandlerSon\n");
    if (signum == SIGALRM)
    {
        // manda mensagem no pipe para pai e signal para ler pipe e mudar estado
        write(pidPipeAlarm[1], pipeNumb, strlen(pipeNumb));
        kill(getppid(), SigMaxExec);
        //Mata o filho e o pai
        kill(pidSon, SIGKILL);
        kill(getpid(), SIGKILL);
    }
}

//Handler de max inactividade para processo pai
void maxInacHandlerGrandFather(int signum)
{
    fprintf(stderr,"maxInacHandlerGrandFather\n");
    if (signum == SigMaxInac)
    {
        char buffer[BUFFER_SIZE];
        //Le do pipe qual tarefa a tratar
        if (readln(pidPipeAlarm[0], buffer, BUFFER_SIZE))
        {
            //Altera o estado da tarefa
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

//Handler de max inactividade para processo filho
void maxInacHandlerFather(int signum)
{
    fprintf(stderr,"maxInacHandlerFather\n");
    if (signum == SigMaxInac)
    {
        // manda mensagem no pipe para pai e signal para ler pipe e mudar estado
        write(pidPipeAlarm[1], pipeNumb, strlen(pipeNumb));
        kill(getppid(), SigMaxInac);
        //mata pai e filho
        kill(pidSon, SIGKILL);
        kill(getpid(), SIGKILL);
    }
}

//Handler de max inactividade para processos netos
void maxInacHandlerSon(int signum)
{
    fprintf(stderr,"maxInacHandlerSon\n");
    if (signum == SIGALRM)
    {
        int pid = getpid();
        //Mata os processos netos e sinaliza o filho para executar o handler de max inactividade
        if ((pidSon != 0) && (pid != pidSon))
            kill(pidSon, SigTerm);
        kill(getppid(), SigMaxInac);
    }
}

//Handler de terminar
void terminarHandler(int signum)
{
    fprintf(stderr,"terminarHandle\n");
    if (signum == SigTerm)
    {
        //Mata os processos envolvido na tarefa
        int pid = getpid();
        if ((pidSon != 0) && (pid != pidSon))
            kill(pidSon, SigTerm);
        kill(pid, SIGKILL);
    }
}

//Função de leitura do ficheiro log.idx (carregamento inicial)
void readFromLog(Instructions *tarefas)
{
    //Abre o ficheiro de index para os logs
    int logIDX = open("log.idx", O_RDWR | O_CREAT | O_APPEND, 0666);
    char buffer[BUFFER_SIZE];
    //Le o ficheiro linha a linha
    while (readln(logIDX, buffer, BUFFER_SIZE))
    {
        char *args[4];
        char *string;
        int j = 0;
        //Separa a informação de cada linha
        string = strtok(buffer, "#");
        while (string != NULL)
        {
            args[j++] = string;
            string = strtok(NULL, "#");
        }
        //Adiciona a tarefa lida do ficheiro log.idx há estrutura
        *tarefas = addTarefa(*tarefas, atoi(args[0]), args[1], 0);
    }
}

int main(int argc, char const *argv[])
{
    //Inicia a estrutura como null
    tarefas = NULL;
    //Carrega a estrutura apartir do fichero de log.idx
    readFromLog(&tarefas);
    //Criacao de signals para conclusao, max execucao, max inactividade
    signal(SigConclude, concludeInstructionsHandler);
    signal(SigMaxExec, maxExecHandlerFather);
    signal(SigMaxInac, maxInacHandlerGrandFather);
    //Inicializacao de pipes de comunicacao entre processos
    pipe(pidPipeConclude);
    pipe(pidPipeAlarm);
    int fd;
    int lineChars;
    char buffer[BUFFER_SIZE];

    //Cria o pipe com nome de comunicacao entre cliente servidor
    if (mkfifo(FIFO_NAME, 0666) == -1)
    {
        perror("mkfifo error");
    }

    while (1)
    {
        //Abre o pipe com nome de comunicacao entre cliente servidor
        if ((fd = open(FIFO_NAME, O_RDWR)) == -1)
        {
            perror("Erro ao abrir fifo");
            return -1;
        }
        else
        {
            fprintf(stderr, "Fifo aberto para leitura!\n");
            //Le o comando pedido pelo cliente
            lineChars = read(fd, buffer, BUFFER_SIZE);
            buffer[lineChars] = '\0';
            fprintf(stderr, "%s", buffer);
            //Interpreta comando pedido
            interpretMessage(&tarefas, buffer);
            //Imprime Estrutura com as tarefas (Debug)
            fprintf(stderr,"Tempo exec: %d\n",maxExec);
            fprintf(stderr,"Tempo inac: %d\n",maxInac);
            printInstructions(tarefas);
            close(fd);
        }
    }

    return 0;
}
