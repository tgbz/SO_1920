typedef struct comandoSessao{
  int pid;
  char cmd;
} cmdSessao;

int lePedido(char *comando, cmdSessao cmd){
  int pid, erro;
  char *str;
  erro = 0;
  pid = atoi(strtok(comando, " "));
  str = strtok(NULL, " ");
  if (pid > 0){
    cmd.pid = pid;
    if (str[0] == 'B' || str[0] == 'R')
      cmd.cmd = str[0];
    else
      erro = 2; /*erro de cmd*/
  }
  else
    erro = 1; /*erro de pid*/
  return erro;
}
  
int main(){	
  int fd, loopT, pid, i, maxCli, status, erro;
  char *servidorFIFOPATH = "teste/fifos/srv";
  char buffer[128];
  cmdSessao cmd;
  
  erro = 0;
  loopT = 1;
  signal(SIGINT,sigHandler);
  maxCli=0;
  pid=0;
    
  if ((mkfifo(servidorFIFOPATH, 0422)) == 0) {
     printf("arrancou servidor com id: %d\n", getpid());
  }
  else {
    printf("Erro de FIFO");
    loopT = 0;
  }
  
  while(loopT){
    for (i=0; i<128; i++) buffer[i] = '\0'; /*clear buffer*/
    fd = open(servidorFIFOPATH, O_RDONLY);
    while (read(fd, buffer, 128) > 0) {
      erro = lePedido(buffer, cmd);
    }
    close(fd);
    
    if (erro == 0 && maxCli < 5){
      maxCli++;
      if(fork()==0){
        printf("criaSessao(cmd.pid, cmd.comando);\n");
        _exit(0);
      }
      else {
        wait(&status);
        maxCli--;
      }
    }
  }
  unlink(servidorFIFOPATH);
  return 0;
}

