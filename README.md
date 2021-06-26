## OS_Lab 2021


**文件读写编程题目**

**myecho.c**

- myecho.c的功能与系统echo程序相同
- 接受命令行参数，并将参数打印出来，例子如下：
```shell
    $ ./myecho x
    x
    $ ./myecho a b c
    a b c
```
**mycat.c**

- mycat.c的功能与系统cat程序相同
- mycat将指定的文件内容输出到屏幕，例子如下：
- 要求使用系统调用open/read/write/close实现
```shell
    $ cat /etc/passwd 
    root:x:0:0:root:/root:/bin/bash
    daemon:x:1:1:daemon:/usr/sbin:/usr/sbin/nologin
    bin:x:2:2:bin:/bin:/usr/sbin/nologin
    ...
    $ ./mycat /etc/passwd 
    root:x:0:0:root:/root:/bin/bash
    daemon:x:1:1:daemon:/usr/sbin:/usr/sbin/nologin
    bin:x:2:2:bin:/bin:/usr/sbin/nologin
    ...
```
**mycp.c**

- mycp.c的功能与系统cp程序相同
- 将源文件复制到目标文件，例子如下：
- 要求使用系统调用open/read/write/close实现
```shell
    $ cat /etc/passwd
    root:x:0:0:root:/root:/bin/bash
    daemon:x:1:1:daemon:/usr/sbin:/usr/sbin/nologin
    bin:x:2:2:bin:/bin:/usr/sbin/nologin
    ...
    $ ./mycp /etc/passwd passwd.bak 
    $ cat passwd.bak
    root:x:0:0:root:/root:/bin/bash
    daemon:x:1:1:daemon:/usr/sbin:/usr/sbin/nologin
    bin:x:2:2:bin:/bin:/usr/sbin/nologin
    ...
```

**多进程题目**

**mysys.c**: 实现函数mysys，用于执行一个系统命令，要求如下:

- mysys的功能与系统函数system相同，要求用进程管理相关系统- 调用自己实现一遍
- 使用fork/exec/wait系统调用实现mysys
- 不能通过调用系统函数system实现mysys

测试程序:

```c
    #include <stdio.h>

    void mysys(char *command)
    {
        实现该函数，该函数执行一条命令，并等待该命令执行结束
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

    测试程序的输出结果

    --------------------------------------------------
    HELLO WORLD
    --------------------------------------------------
    bin    core  home	     lib	 mnt   root  snap  tmp	vmlinuz
    boot   dev   initrd.img      lost+found  opt   run   srv   usr	vmlinuz.old
    cdrom  etc   initrd.img.old  media	 proc  sbin  sys   var
    --------------------------------------------------
```

**sh1.c**

- 该程序读取用户输入的命令，调用函数mysys(上一个作业)执行用户的命令，示例如下:

```shell
    # 编译sh1.c
    $ cc -o sh1 sh1.c

    # 执行sh1
    $ ./sh 

    # sh1打印提示符>，同时读取用户输入的命令echo，并执行输出结果
    > echo a b c
    a b c

    # sh1打印提示符>，同时读取用户输入的命令cat，并执行输出结果
    > cat /etc/passwd
    root:x:0:0:root:/root:/bin/bash
    daemon:x:1:1:daemon:/usr/sbin:/usr/sbin/nologin
    bin:x:2:2:bin:/bin:/usr/sbin/nologin
```
- 请考虑如何实现内置命令cd、pwd、exit 

**sh2.c**: 实现shell程序，要求在第1版的基础上，添加如下功能实现文件重定向

```shell
    # 执行sh2
    $ ./sh2

    # 执行命令echo，并将输出保存到文件log中
    > echo hello >log

    # 打印cat命令的输出结果
    > cat log
    hello
```
**sh3.c**: 实现shell程序，要求在第2版的基础上，添加如下功能

- 实现管道
- 只要求连接两个命令，不要求连接多个命令
- 不要求同时处理管道和重定向
```shell
    # 执行sh3
    $ ./sh3

    # 执行命令cat和wc，使用管道连接cat和wc
    > cat /etc/passwd | wc -l
```

- 考虑如何实现管道和文件重定向，暂不做强制要求
```shell
    $ cat input.txt
    3
    2
    1
    3
    2
    1
    $ cat <input.txt | sort | uniq | cat >output.txt
    $ cat output.txt
    1
    2
    3
```
**多线程题目**
**pi1.c**: 使用2个线程根据莱布尼兹级数计算PI

- 莱布尼兹级数公式: 1 - 1/3 + 1/5 - 1/7 + 1/9 - ... = PI/4
- 主线程创建1个辅助线程
- 主线程计算级数的前半部分
- 辅助线程计算级数的后半部分
- 主线程等待辅助线程运行結束后,将前半部分和后半部分相加

**pi2.c**: 使用N个线程根据莱布尼兹级数计算PI

- 与上一题类似，但本题更加通用化，能适应N个核心
- 主线程创建N个辅助线程
- 每个辅助线程计算一部分任务，并将结果返回
- 主线程等待N个辅助线程运行结束，将所有辅助线程的结果累加
- 本题要求 1: 使用线程参数，消除程序中的代码重复
- 本题要求 2: 不能使用全局变量存储线程返回值 

**sort.c**: 多线程排序

- 主线程创建两个辅助线程
- 辅助线程1使用选择排序算法对数组的前半部分排序
- 辅助线程2使用选择排序算法对数组的后半部分排序
- 主线程等待辅助线程运行結束后,使用归并排序算法归并子线程的计算结果
- 本题要求 1: 使用线程参数，消除程序中的代码重复 

**pc1.c**: 使用条件变量解决生产者、计算者、消费者问题

- 系统中有3个线程：生产者、计算者、消费者
- 系统中有2个容量为4的缓冲区：buffer1、buffer2
- 生产者生产'a'、'b'、'c'、‘d'、'e'、'f'、'g'、'h'八个字符，放入到buffer1
- 计算者从buffer1取出字符，将小写字符转换为大写字符，放入到buffer2
- 消费者从buffer2取出字符，将其打印到屏幕上

**pc2.c**: 使用信号量解决生产者、计算者、消费者问题
- 功能和前面的实验相同，使用信号量解决


#### 考试方式

- 上机考试，环境为在线 Linux
- 共 4 道题，2 道题多线程，2 道题多进程
- 考试时可以查看任意的纸质资料、课程网站的讲义
- 考试时不可以访问课程网站之外的网站
- 考试时不可以使用 QQ
- 考试时不可以使用手机 

##### 上机考试的注意事项

- 每个人的题目不完全一样，例如
    
    1.张三的第 1 题是计算从1加到100的累加和，第 2 题是计算 N 的阶乘，第 3 题是计算斐波拉切数列 
    
    2.李四的第 1 题是计算计算 N 的阶乘，第 2 题是找到数组中的最大值，第 3 题是对数组排序 

- 只能在指定的时间段下载和上传题目，例如
        
        1.在 09:00 - 09:15 的时间段，下载和上传第 1 题
        
        2.在 09:15 - 09:30 的时间段，下载和上传第 2 题
        
        3.在 09:30 - 09:45 的时间段，下载和上传第 3 题
        
        4.在 09:45 - 10:00 的时间段，下载和上传第 4 题 

- **没有在指定时间段上传题目，该题无分**

- **在指定时间段上传的题目与该时间段不对应，该题无分**，例如

    1.发给你的题目序列是：第 1 题是计算从1加到100的累加和，第 2 题是计算 N 的阶乘，第 3 题是计算斐波拉切数列
    
    2.**系统会记录下发给你的题目序列**
    
    3.改卷子时，会人工检查，你提交的第 1 题是否是完成计算从1加到100的累加和，如果不是，该题无分
    
    4.如果你提交的题目是第 1 题是计算 N 的阶乘，第 2 题是计算斐波拉切数列，第 3 题是计算从1加到100的累加和，因为与给你的题目序列不匹配，即使你提交的 3 道题是完全正确的，也是没有任何得分的。 

**2021上机题目**

 **T1**:
 实现命令 mygrep string file
 + 该命令逐行读取文件，如果行包括目标字符串，则打印该行
 + 该命令有两个命令行参数
   - 参数 string，要搜索的字符串
   - 参数 file，要查找的文件名
 
 例子，在文件 /etc/passwd 中查找字符串 root，打印包含该字符串的行
 
 ```shell
 
 $ ./mygrep root /etc/passwd
 root:x:0:0:root:/root:/bin/bash
 
 ```
 
 提示，可以使用函数 strstr 在字符串中查找字符串
 https://www.runoob.com/cprogramming/c-function-strstr.html
 
 **T2**:
 通过 fork/exec/pipe/dup 实现 cat </etc/passwd | wc -l >result.txt 
 + 父进程创建子进程
   - 在父进程中实现功能 cat </etc/passwd
   - 在子进程中实现功能 wc -l >result.txt
   - 不能实现为
     * 在子进程中实现功能 cat </etc/passwd
     * 在父进程中实现功能 wc -l >result.txt

 + 该题不要求实现一个通用的 shell 程序
   - 不需要使用 strtok 对字符串进行分割处理
   - 假设字符串已经分割处理好了
     * 父进程，使用 execlp("cat") 执行命令，使用 open("/etc/passwd") 打开文件
     * 子进程，使用 execlp("wc") 执行命令，使用 open("result.txt") 打开文件

 + 请严格按要求完成
   - 把作业 sh3.c 的内容复制过来，是没有分数的

 **T3**:
 + 实现函数 list_visit
   - 主程序遍历链表 list 的每个节点
   - 对每个节点创建一个子线程，在这道题目中，总共创建 7 个子线程
   - 在每个子线程中，打印 node->data

 + 实现函数 wait_sub_threads
   - 等待所有的子线程执行完毕
   - 在 list_visit 时，需要记录下所有线程的 ID，记录在全局变量中
   - 在 wait_sub_threads 中使用 pthread_join

 + 程序可能的输出结果
   - 字符 a b c d e f g 每个出现一次，出现的顺序可能是随机的  
   - END 一定是最后出现
   a
   d
   b
   e
   c
   f
   g
   END
   
```c
#include <stdio.h>
#include <stdlib.h>

struct node {
    char data;
    struct node *next;
};

struct node *node_create(char data)
{
    struct node *node = malloc(sizeof(struct node));
    node->data = data;
    node->next = NULL;
    return node;
}

void list_visit(struct node *list)
{
}

// 遍历算法，供参考
void list_print(struct node *list)
{
    struct node *node = list;
    while (node) {
        printf("%c\n", node->data);
        node = node->next;
    }
}

void wait_sub_threads()
{
}

int main()
{
    struct node *list = NULL;
    for (char data = 'g'; data >= 'a'; data--) {
        struct node *node = node_create(data);
        node->next = list;
        list = node;
    }

    list_print(list);
    wait_sub_threads();

    puts("END");
    return 0;
}
```

 **T4**:
  + 补全生产者、计算者、消费者程序
   - 完成函数 buffer_init
   - 完成函数 buffer_get
   - 完成函数 buffer_put
   - 补全函数 main
     * 创建生产者、计算者、消费者

 + 程序最终输出
   - 生产者输出 a b c d e f g h
   - 消费者输出 A B C D E F G H
   - 消费者输出 A B C D E F G H
   - 两者交叉输出 

 + 不准对函数 produce 、函数 compute、 函数 consume 进行任何改动
   - 如果改动，则该题没有分数

 + 本题对 buffer 进行了抽象和封装
   - buffer_pc 连接 producer 和 computer
   - buffer_cc 连接 computer 和 consumer 

```c
#include <stdio.h>
#include <pthread.h>
#include <ctype.h>

#define CAPACITY 4      // 缓冲区的最大容量

struct buffer {
    int data[CAPACITY];
    int in;             // 缓冲区的写指针
    int out;            // 缓冲区的读指针
    pthread_mutex_t mutex;
    pthread_cond_t wait_empty_buffer;
    pthread_cond_t wait_full_buffer;
};

// 初始化 buffer，需要初始化用于同步的字段 
void buffer_init(struct buffer *buffer)
{
    buffer->in = 0;
    buffer->out = 0;
}

// 判断缓冲区是否为空
int buffer_is_empty(struct buffer *buffer)
{
    return buffer->in == buffer->out;
}

// 判断缓冲区是否为满
int buffer_is_full(struct buffer *buffer)
{
    return (buffer->in + 1) % CAPACITY == buffer->out;
}

// 向缓冲区中追加一个数据，如果缓冲区为满，则等待
void buffer_put(struct buffer *buffer, int item)
{
}

// 从缓冲区中取走一个数据，如果缓冲区为空，则等待
int buffer_get(struct buffer *buffer)
{
}

#define ITEM_COUNT (2 * CAPACITY)

struct buffer buffer_pc, buffer_cc;

void *produce(void *arg)
{
    int i;
    int item;

    for (i = 0; i < ITEM_COUNT; i++) { 
        item = 'a' + i;
        printf("produce item: %c\n", item);
        buffer_put(&buffer_pc, item);
    }
    return NULL;
}

void *compute(void *arg)
{
    int i;
    int item;

    for (i = 0; i < ITEM_COUNT; i++) { 
        item = buffer_get(&buffer_pc); 
        item = toupper(item);
        printf("    compute item: %c\n", item); 
        buffer_put(&buffer_cc, item);
    }
    return NULL;
}

void *consume(void *arg)
{
    int i;
    int item;

    for (i = 0; i < ITEM_COUNT; i++) { 
        item = buffer_get(&buffer_cc); 
        printf("        consume item: %c\n", item); 
    }
    return NULL;
}

int main()
{ 
    buffer_init(&buffer_pc);
    buffer_init(&buffer_cc);

    return 0;
}
```
 
