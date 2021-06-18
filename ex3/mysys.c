#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include<unistd.h>
#include<sys/wait.h>

void mysys(char *command)
{
	int pid;
	pid=fork();
	if(pid==0){
    char **dst;
    char* in;
    int l=strlen(command);
    in=(char*)malloc(sizeof(char)*l);
    strncpy(in,command,l);
    const char *delim = " ";
    char *p;
    char **line;
    int pos;
    char *index = strrchr(in, ' ');
    if (!index)
        line = (char **)malloc(sizeof(char *) * 1);
    else
    {
        pos = (index - in) / sizeof(char);
        int len = strlen(in) / 2 + 1;
        line = (char **)malloc(sizeof(char *) * len);
    }
    p = strtok(in, delim);
    int cnt = 0;
    while (p != NULL)
    {
        line[cnt] = (char *)malloc(sizeof(char) * strlen(p));
        strcpy(line[cnt++], p);
        p = strtok(NULL, delim);
    }
    line[cnt] = NULL;
    dst = (char **)malloc(sizeof(char *) * (cnt + 1));
    for (int i = 0; i < cnt; i++)
    {
        dst[i] = (char *)malloc(sizeof(char) * strlen(line[i]));
        strcpy(dst[i], line[i]);
    }
    dst[cnt] = NULL;
	int error=execvp(dst[0],dst);
	if(error<0) perror("execv");
	}
	wait(NULL);
}

int main()
{
    printf("--------------------------------------------------\n");
    mysys("echo HELLO WORLD");
    printf("--------------------------------------------------\n");
    mysys("ls /");
    printf("--------------------------------------------------\n");
    return 0;
}
