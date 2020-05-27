typedef struct comandoBackup{
  char ficheiro[256];
  char permissoes[4]
} cmdBackup;

typedef struct comandoRestore{
  char ficheiro[256];
  char permissoes[4]
} cmdRestore;

int criaSessao(int pid, char tipo){
  if (tipo == 'B')
    return 1;
  else
    return 0;  
}

int iniciaSessaoRX(int pid){
  int fd, loopT, pid, i, flag;
  char servidorFIFODados[128];
  char servidorFIFOComando[128];
  char buffer[128];
  char *str, *ficheiro, *cmd;
  char *servidorFIFOPATH = "teste/fifos/";
  
  sprintf(servidorFIFODados, "%s%dD", servidorFIFOPATH, ID);
  sprintf(servidorFIFOComando, "%s%dC", servidorFIFOPATH, ID);
  
  loopT = 1;
  flag = 1;
  pid = 0;
  signal(SIGINT,sigHandler);
  if ((mkfifo(servidorFIFOComando, 0666)) == 0) {
     kill(ID,SIGUSR1); /*cliente pode comecar a copiar ficheiros */
     printf("arrancou servidor com id: %d\n", getpid());
  }
  else {
    printf("Erro de FIFO");
    loopT = 0;
    kill(pid,SIGUSR2);
  }
  
  while(loopT){
    fd = open(servidorFIFOComando, O_RDONLY);
/*    for (i=0; i<128; i++) buffer[i] = '\0'; */
    while (read(fd, buffer, 128) > 0) {
      str = strtok(buffer, "\r\n");
      pid = atoi(strtok(str, " "));
      cmd = strtok(NULL, " ");
      ficheiro = strtok(NULL, "\0");
    }
    if(strcmp(cmd, "S") == 0){
      loopT=0;
      flag=0;
      printf("######## Sessao Fechada ########\n");
    }
    if (pid!=0){
      close(fd);
      kill(pid,SIGUSR1);
      printf("Recebi do pid %d o comando %s para o ficheiro %s\n", pid, cmd, ficheiro);
      if (flag)
        executaComando(cmd, pid, ficheiro);
      printf("Enviou Sinal %d para o pid %d\n", SIGUSR1, pid);
      pid = 0;
    }
  }
  unlink(servidorFIFOComando);
  return 0;
}
