/* pipe() */
#define _GNU_SOURCE             /* See feature_test_macros(7) */

/* pipe(), fork(), dup2(), close(), execlp(), read(), write() precisam das seguintes libs */
#include <unistd.h>

/* pipe() precisa das seguintes libs */
#include <fcntl.h>              /* Obtain O_* constant definitions */
#include <unistd.h>

#include <string.h>
#include <stdlib.h>

char *calculaSha1Sum(char* nomeFicheiro){
	int fd[2], nBytesLidos;
	char buff[128];
	char *str;
  pipe(fd);
	if(fork()==0){
	  dup2(fd[1],1);
		close(fd[0]);
		close(fd[1]);
    execlp("sha1sum", "sha1sum", nomeFicheiro, NULL);
  }
  else{
    close(fd[1]);
    nBytesLidos = read(fd[0], buff, 128);
    str = malloc(sizeof(char) * (nBytesLidos + 1));
    if(str){
      strcpy(str, buff);
    }
    close(fd[0]);
  }
	return str;
}

char *genericExec3(char* nomeFicheiro){
	int fd1[2], fd2[2], nBytesLidos;
	char buff[1014];
	char *str;
	pipe(fd1);
  if(fork()==0){
    dup2(fd1[1],fd2[1]);
    close(fd1[0]);
    close(fd1[1]);
    pipe(fd2);
  	if(fork()==0){
	    dup2(fd2[1],1);
		  close(fd2[0]);
		  close(fd2[1]);
      execlp("sha1sum", "sha1sum", nomeFicheiro, NULL);
    }
    else{
      close(fd2[1]);
      nBytesLidos = read(fd2[0], buff, 1024);
      execlp("sha1sum", "sha1sum", nomeFicheiro, NULL);
      write(fd1[1], buff, nBytesLidos);
      
      close(fd2[0]);
    }
  }
  else {
    close(fd1[1]);
    nBytesLidos = read(fd1[0], buff, 128);
    str = malloc(sizeof(char) * (nBytesLidos + 1));
    if(str){
      strcpy(str, buff);
    }
    close(fd1[0]);
  }
	return str;
}

char *getFileName(char *file)
{
  char *str, *aux ,res[128];
  int i, status;
  int fd[2], nBytesLidos;
  char buff[128];

  pipe(fd);
  if (fork()==0)
  {
    dup2(fd[1],1);
    close(fd[0]);
    close(fd[1]);
      execlp("readlink","readlink","-f",file, NULL);
      _exit(0);
  }
  else
  {
    wait(&status);
    close(fd[1]);
      nBytesLidos = read(fd[0], buff, 128);
      aux = strtok(buff, "\r\n");
      str = malloc(sizeof(char) * nBytesLidos);
      if(str){
        strcpy(str, aux);
      }
      
      close(fd[0]);
    for (i = strlen(str);  i>0 && str[i]!='/'; i--); 
      strcpy(res,&str[i+1]);
    
  }
  return res;
}

char *getFileAbsolutePath(char *file)
{
  char *str, *aux ,res[128];
  int i, status;
  int fd[2], nBytesLidos;
  char buff[128];

  pipe(fd);
  if (fork()==0)
  {
    dup2(fd[1],1);
    close(fd[0]);
    close(fd[1]);
      execlp("readlink","readlink","-f",file, NULL);
      _exit(0);
  }
  else
  {
    wait(&status);
    close(fd[1]);
      nBytesLidos = read(fd[0], buff, 128);
      aux = strtok(buff, "\r\n");
      str = malloc(sizeof(char) * nBytesLidos);
      if(str){
        strcpy(str, aux);
      }
      
      close(fd[0]);
    for (i = strlen(str);  i>0 && str[i]!='/'; i--);
    for (j = 0; j < i; j++)
    {
      res[j]=str[j];
    }
    res[j]='\0';
  }
  return res;
}

char fromBinToOctal(char ch1,char ch2, char ch3)
{
  if(ch1=='1')
    {
      if (ch2=='1')
      {
        if (ch3=='1') return '7';
        else      return '6';
      }
      else
      {
        if (ch3=='1') return '5';
        else      return '4';
      }
    }
    else
    {
      if (ch2=='1')
      {
        if (ch3=='1') return '3';
        else      return '2';
      }
      else
      {
        if (ch3=='1') return '1';
        else      return '0';
      }
    } 
}

char *getPermitions(char *ficheiro)
{
  int i=0;
  int p[2];
  int pp[2];
  char *bin;
  char oct[4];
  char buf;
  
  pipe(p);
  if (fork()==0)
  {
    pipe(pp);
    if (fork()==0)
      {
        dup2(pp[1],1);
        close(pp[0]);
        close(pp[1]);
        execlp("ls","ls","-l",ficheiro,NULL);
      }
    else
      {
        dup2(pp[0],0);
        dup2(p[1],1);
        close(pp[0]);
        close(pp[1]);
        close(p[0]);
        close(p[1]);
        execlp("awk","awk","{print $1}",NULL);
      } 

  }
  else
  {
    dup2(p[0],0);
    close(p[0]);
    close(p[1]);
    while(read(0,&buf,1)!=0)
    {
      if (buf=='-')
      {
        bin[i]='0';
      }
      else
      {
        bin[i]='1';
      }
      i++;
    }
    
    oct[0]=bin[0];
    oct[1]=fromBinToOctal(bin[1],bin[2],bin[3]);
    oct[2]=fromBinToOctal(bin[4],bin[5],bin[6]);
    oct[3]=fromBinToOctal(bin[7],bin[8],bin[9]);
    return oct;
  }
}

int makeDirectory(char *dirname)
{
  int status;
  if (fork()==0)
  {
    execlp("mkdir","mkdir",argv[1], NULL);
      _exit(0);
  }
  else
  {
    wait(&status);
  }
}

/*arg1 destino, arg2 nome do link(ficheiro)*/
int linker(char *arg1, char *arg2)
{
	int status;
	if (fork()==0)
	{
		execlp("ln","ln","-s",arg1,arg2,NULL);
	}
	else
	{
		wait(&status);
	}
	return 0;	
}
