#include <unistd.h>
#include <unistd.h>
#include <stdio.h>


char *retornaPermissoes(char *ficheiro)
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

		if(bin[1]=='1')
		{
			if (bin[2]=='1')
			{
				if (bin[3]=='1') oct[1]='7';
				else 		oct[1]='6';
			}
			else
			{
				if (bin[3]=='1')oct[1]='5';
				else 		oct[1]='4';
			}
		}
		else
		{
			if (bin[2]=='1')
			{
				if (bin[3]=='1')oct[1]='3';
				else		oct[1]='2';
			}
			else
			{
				if (bin[3]=='1')oct[1]='1';
				else		oct[1]='0';
			}
		}
		if(bin[4]=='1')
		{
			if (bin[5]=='1')
			{
				if (bin[6]=='1') oct[2]='7';
				else 		oct[2]='6';
			}
			else
			{
				if (bin[6]=='1')oct[2]='5';
				else 		oct[2]='4';
			}
		}
		else
		{
			if (bin[5]=='1')
			{
				if (bin[6]=='1')oct[2]='3';
				else		oct[2]='2';
			}
			else
			{
				if (bin[6]=='1')oct[2]='1';
				else		oct[2]='0';
			}
		}
		if(bin[7]=='1')
		{
			if (bin[8]=='1')
			{
				if (bin[9]=='1') oct[3]='7';
				else 		oct[3]='6';
			}
			else
			{
				if (bin[9]=='1')oct[3]='5';
				else 		oct[3]='4';
			}
		}
		else
		{
			if (bin[8]=='1')
			{
				if (bin[9]=='1')oct[3]='3';
				else		oct[3]='2';
			}
			else
			{
				if (bin[9]=='1')oct[3]='1';
				else		oct[3]='0';
			}
		}
	}
	return oct;
}
