#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    struct timeval start, end; //记录时间
    pid_t pid;
    pid = fork();  //创建进程
    if (pid == -1) //如果出错
    {
        printf("Create Failed!\n");
        exit(1);
    }
    if (pid == 0) //位于子进程中
    {
        printf("Create Succeed!\n");
        printf("Running:%s\n", argv[1]);
        execv(argv[1], &argv[1]); //在子进程中调用execv函数在命令行中来运行一个程序
    }
    else //位于当前进程
    {
        gettimeofday(&start, NULL); //获取开始执行时间
        wait(NULL);                 //等待子进程结束
        gettimeofday(&end, NULL);   //获取执行结束时间
        //计算秒和毫秒
        //防止出现负数
        int num = (end.tv_usec - start.tv_usec) + (end.tv_sec - start.tv_sec) * 1000000;
        int seconds = num / 1000000;
        int useconds = (end.tv_usec - start.tv_usec) % 1000000;
        //输出秒和毫秒
        printf("运行时间：%d秒 %d微秒\n", seconds, useconds);
    }
    return 0;
}
