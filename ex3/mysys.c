#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void mysys(char *command)
{
    int pid;
    pid = fork();
    if (pid == 0)
    {
        char **dst;
        char *in;
        int l = strlen(command);

        in = (char *)malloc(sizeof(char) * l);
        strncpy(in, command, l);
        const char *delim = " ";//分隔符用于切割command
        char *p;
        char **line;
        int pos;
        //用strrchr返回字符' '在in中倒数第一次出现的位置(char*指针)
        char *index = strrchr(in, ' ');
        //若未找到则返回NULL，说明command只有一个字符串单位
        //只需要malloc一个单位的char*大小
        if (!index)
            line = (char **)malloc(sizeof(char *) * 1);
        else
        {
            //用少量空间复杂度换时间复杂度，
            //这里我们取字符数量一半作为字符串数组的容量
            int len = strlen(in) / 2 + 1;
            // printf("设置最大分割单位数量为%d个\n",len);
            line = (char **)malloc(sizeof(char *) * len);
        }
        //strtok会破坏原字符串in,所以必须用line数组来存储分割后的字符串
        p = strtok(in, delim);
        //设置一个cnt计数器保存line数组实际存储的command分割后的字符串数量
        int cnt = 0;
        //strtok返回下一个分割后的字符串指针
        //结束时返回NULL，故以此作为循环结束条件
        while (p != NULL)
        {
            //前面我们为char**的line申请了len个单位来保存char*指针
            //此处每当分割获取到一个单位的字符串，就strlen返回p长度，
            //按照该值为line数组中每个字符串指针分配所需空间
            line[cnt] = (char *)malloc(sizeof(char) * strlen(p));
            //将分割下来的字符串的单位复制到对应的line数组的元素中
            strcpy(line[cnt++], p);
            //继续分割命令字符串，delim为分隔符，即' '
            p = strtok(NULL, delim);
        }
        //execv命令所需的参数数组要保证末尾为NULL，
        //而cnt的作用就在这里：
        line[cnt] = NULL;
        //将argv参数数组作为输入调用execvp
        int error = execvp(line[0], line);
        if (error < 0)
            perror("execv");
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
