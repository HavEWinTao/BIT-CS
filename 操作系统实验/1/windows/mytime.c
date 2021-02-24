#include <stdio.h>
#include <windows.h>

int main(int argc, char *argv[])
{
    SYSTEMTIME start, end; //记录时间
    STARTUPINFO startinfo; //lpStartUpInfo参数
    ZeroMemory(&startinfo, sizeof(startinfo));
    startinfo.cb = sizeof(startinfo);
    PROCESS_INFORMATION procinfo; //lpProcessInformation参数
    TCHAR cmd[MAX_PATH];          //lpCommandLine参数
    sprintf(cmd, "%s", argv[1]);
    //创建进程
    if (!CreateProcess(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &startinfo, &procinfo))
    {
        printf("Create Failed!\n");
        //创建失败退出
        exit(1);
    }
    printf("Create Succeed!\n");
    printf("Running:%s\n", argv[1]);
    //记录开始时间
    GetSystemTime(&start);
    //等待进程同步
    WaitForSingleObject(procinfo.hProcess, INFINITE);
    //记录结束时间
    GetSystemTime(&end);
    //提取出分、秒、毫秒
    //防止出现负数
    int num = end.wMilliseconds - start.wMilliseconds;
    num += (end.wSecond - start.wSecond) * 1000;
    num += (end.wMinute - start.wMinute) * 60 * 1000;
    int minute = num / 60 / 1000;
    int sec = num / 1000;
    int msec = num - minute * 60 * 1000 - sec * 1000;
    //输出运行时间
    printf("cost time: %dminutes %dseconds %dmilliseconds", minute, sec, msec);
    return 0;
}