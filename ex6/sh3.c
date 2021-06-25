#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#define SIZE 128
char *strs[SIZE];
int len;//管道连接的命令数量
struct passwd *pwd;
int STDIN, STDOUT;
void input(char *, char *[]);

void prompt()
{
	char buf[SIZE];
	getcwd(buf, SIZE);
	pwd = getpwuid(getuid());
	//打印出用户名和当前工作路径，模拟shell的输出
	printf("%s@Void7:%s$ ", pwd->pw_name, buf);
	//保证输出干净
	fflush(stdout);
}

char t_in[2 * SIZE];

//外部指令的执行框架函数
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
	//将之前在父进程重定向的标准IO描述符重定向回去，保证后续命令的IO
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

void exec_pipe(int index)
{
	// printf("index=%d\n", index);
	char *line[2 * SIZE];
	int fd_array[2];
	pipe(fd_array);
	int pid = fork();
	if (pid == 0)
	{
		//除最后一条命令，都需要将标准输出重定向到管道
		if (index != len - 1)
		{
			dup2(fd_array[1], 1);
			close(fd_array[0]);
			close(fd_array[1]);
		}
		//分割第index条命令到line数组中
		input(strs[index], line);
		//用exec来执行line参数数组
		exec_simple(line);
		//退出后其父进程即可继续fork出执行下一条命令的进程
		exit(0);
	}
	if (pid < 0)
	{
		perror("fork");
		return;
	}
	//必须在fork出下一条命令前重定向标准输入到管道，这样子进程才能继承
	dup2(fd_array[0], 0);
	close(fd_array[0]);
	close(fd_array[1]);
	//等待前一个fork出的命令子进程
	waitpid(pid, NULL, 0);
	//递归结束条件
	if (index != len - 1)
		exec_pipe(index + 1);
	//该进程是sh3主进程fork出的命令进程，同样需要结束后让sh3停止阻塞等待
	exit(0);
}

//以管道连接符作为分割并计算共有几条，存入len
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
		strs[i] = (char *)malloc(sizeof(char) * strlen(token));
		strcpy(strs[i++], token);
		token = strtok(NULL, "|\n");
	}
	strs[i] = NULL;
	len = i;
}

void input(char *command, char *s[])
{
	char *cmd;
	cmd = (char *)malloc(sizeof(char) * strlen(command));
	strcpy(cmd, command);
	char *token;
	int i = 0;
	token = strtok(cmd, " \n");
	while (token != NULL)
	{
		int fd = -1;
		//重定向输入
		if (token[0] == '<')
		{
			if (token[1] != ' ' && token[1])
				fd = open(token + 1, O_RDONLY, 0666);
			else if (!token[1])
			{
				token = strtok(NULL, " \n");
				fd = open(token,  O_RDONLY, 0666);
			}
			if (fd < 0)
			{
				perror("fd");
				return;
			}
			//先在父进程中进行了重定向，这样后面fork出的子进程直接继承使用
			dup2(fd, 0);
			close(fd);
			break;
		}
		//重定向输出
		else if (token[0] == '>')
		{
			//'>'符号后面无空格的情况，token[1]即token+1就是文件名
			if (token[1] != ' ' && token[1])
				fd = open(token + 1, O_CREAT | O_RDWR, 0666);
			//'>'符号后面是空格的情况，需要再执行一次strtok
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
		s[i] = (char *)malloc(sizeof(char) * strlen(token));
		strcpy(s[i++], token);
		token = strtok(NULL, " \n");
	}
	s[i] = NULL;
}

int main()
{
	//后面对单个命令进行分割时，由于是先在父进程先进行重定向，
	//需要先用两个文件描述符保存下标准IO，以便命令执行后sh3后续IO能够正常进行
	STDIN = dup(0);
	STDOUT = dup(1);
	while (1)
	{
		prompt();
		parse();
		//存在管道连接的命令
		if (len > 1)
		{
			// printf("共%d个命令用管道连接\n", len);
			int pid = fork();
			if (pid == 0)
				exec_pipe(0);
			if (pid < 0)
			{
				perror("fork");
				return -1;
			}
			else
				waitpid(pid, NULL, 0);
		}
		else
		{ //不存在管道连接的命令
			char *line[2 * SIZE];
			input(t_in, line);
			exec_simple(line);
		}
	}
	return 0;
}
