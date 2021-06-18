#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#define SIZE 512
char *str[SIZE];
char *strs[SIZE];
int len;
struct passwd *pwd;
int STDIN, STDOUT;
void input(char *, char *[]);

void prompt()
{
	char buf[SIZE];
	getcwd(buf, SIZE);
	pwd = getpwuid(getuid());
	printf("%s@Void7:%s$ ", pwd->pw_name, buf);
	fflush(stdout);
}

char t_in[2 * SIZE];

void external(char *s[])
{
	//printf("external s[0]: %s\n", s[0]);
	int pid = fork();
	if (pid == 0)
	{
		int e = execvp(s[0], s);
		if (e < 0)
			perror("execvp");
	}
	waitpid(pid, NULL, 0);
	dup2(STDIN, 0);
	dup2(STDOUT, 1);
}
void i_cd(char *s[])
{
	int e = chdir(s[1]);
	if (e < 0)
		perror("chdir");
}
void i_pwd()
{
	char buf[SIZE];
	getcwd(buf, SIZE);
	printf("%s\n", buf);
}

void exec_simple(char *s[])
{
	//printf("exec_simple s[0]: %s\n", s[0]);
	if (!strcmp(s[0], "cd"))
	{
		i_cd(s);
	}
	else if (!strcmp(s[0], "pwd"))
	{
		i_pwd();
	}
	else if (!strcmp(s[0], "exit"))
	{
		exit(0);
	}
	else
		external(s);
}

void exec_pipe()
{
	char *line[2 * SIZE];
	int fd_array[2];
	pipe(fd_array);
	int pid = fork();
	if (pid == 0)
	{
		//printf("进入子进程pid=%d,fd[1]=%d\n", pid, fd_array[1]);
		dup2(fd_array[1], 1);
		close(fd_array[0]);
		close(fd_array[1]);
		input(strs[0], line);
		exec_simple(line);
		exit(0);
	}
	if (pid < 0)
	{
		perror("fork");
		return;
	}
	dup2(fd_array[0], 0);
	close(fd_array[0]);
	close(fd_array[1]);
	waitpid(pid, NULL, 0);
	input(strs[1], line);
	//printf("父进程exec_pipe strs[1]: %s\n", strs[1]);
	//printf("父进程exec_pipe line[0]: %s\n", line[0]);
	exec_simple(line);
}

//判断是否使用管道连接两个命令
void parse()
{
	int cnt = read(0, t_in, sizeof(t_in));
	t_in[cnt] = 0;
	memset(strs, 0, 2 * SIZE);
	char *token;
	int i = 0;
	token = strtok(t_in, "|\n");
	while (token != NULL)
	{
		strs[i] = (char *)malloc(sizeof(char)*strlen(token));
		strcpy(strs[i++], token);
		token = strtok(NULL, "|\n");
	}
	strs[i] = NULL;
	len = i;
}

void input(char *command, char *s[])
{
	char *cmd;
	cmd = (char *)malloc(sizeof(char)*strlen(command));
	strcpy(cmd, command);
	char *token;
	int i = 0;
	token = strtok(cmd, " \n");
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
		s[i] = (char *)malloc(sizeof(char)*strlen(token));
		strcpy(s[i++], token);
		token = strtok(NULL, " \n");
	}
	s[i] = NULL;
	len = i;
}

int main()
{
	STDIN = dup(0);
	STDOUT = dup(1);
	while (1)
	{
		prompt();
		parse();
		if (len > 1)
		{ //使用管道连接两个命令
			int pid = fork();
			if (pid == 0)
				exec_pipe();
			if (pid < 0)
			{
				perror("fork");
				return -1;
			}
			waitpid(pid, NULL, 0);
		}
		else
		{ //未使用管道
			char *line[2 * SIZE];
			input(t_in, line);
			exec_simple(line);
		}
	}
	return 0;
}
