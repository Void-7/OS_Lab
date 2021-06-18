#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#define SIZE 256
char *str[SIZE];
int len;
struct passwd *pwd;
int STDIN,STDOUT;
void prompt()
{
	char buf[SIZE];
	getcwd(buf, SIZE);
	pwd = getpwuid(getuid());
	printf("%s@Void7:%s$ ", pwd->pw_name, buf);
	fflush(stdout);
}

void input()
{
	char t_in[2 * SIZE];
	int cnt = read(0, t_in, sizeof(t_in));
	t_in[cnt] = 0;
	memset(str, 0, 2 * SIZE);
	char *token;
	int i = 0;
	token = strtok(t_in, " \n");
	while (token != NULL)
	{
		int fd = -1;
		if (token[0] == '>')
		{
			if (token[1] != ' ' && token[1])
				fd = open(token + 1, O_CREAT | O_RDWR, 0666);
			else if (!token[1])
			{
				token = strtok(NULL, " \n");
				fd = open(token, O_CREAT | O_RDWR, 0666);
			}
			if (fd < 0)
			{
				perror("fd");
				return;
			}
			dup2(fd, 1);
			close(fd);
			break;
		}
		str[i] = (char *)malloc(sizeof(char)*strlen(token));
		strcpy(str[i++], token);
		token = strtok(NULL, " \n");
	}

	str[i] = NULL;
	len = i;
}

void external()
{
	int pid = fork();
	if (pid == 0)
	{
		int e = execvp(str[0], str);
		if (e < 0)
			perror("execvp");
	}
	wait(NULL);
	dup2(STDIN, 0);
	dup2(STDOUT, 1);
}
void i_cd()
{
	int e = chdir(str[1]);
	if (e < 0)
		perror("chdir");
}
void i_pwd()
{
	char buf[SIZE];
	getcwd(buf, SIZE);
	printf("%s\n", buf);
}

int main()
{
	STDIN=dup(0);
	STDOUT=dup(1);
	while (1)
	{
		prompt();
		input();
		if (!strcmp(str[0], "cd"))
		{
			i_cd();
		}
		else if (!strcmp(str[0], "pwd"))
		{
			i_pwd();
		}
		else if (!strcmp(str[0], "exit"))
		{
			exit(0);
		}
		else
			external();
	}
	return 0;
}

