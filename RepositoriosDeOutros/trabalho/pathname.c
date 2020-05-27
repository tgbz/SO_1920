#define _GNU_SOURCE             /* See feature_test_macros(7) */

/* pipe(), fork(), dup2(), close(), execlp(), read(), write() precisam das seguintes libs */
#include <unistd.h>

/* pipe() precisa das seguintes libs */
#include <fcntl.h>              /* Obtain O_* constant definitions */
#include <unistd.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


char* main(char *arg)
{
	char *str,res[128];
	int i,j;
	str=strdup(arg);
	for (i = strlen(str);  i>0 && str[i]!='/'; i--);
    for (j = 0; j < i; j++)
    {
    	res[j]=str[j];
    }
    res[j+1]='\0';
	return res;
}