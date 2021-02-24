#include "cd.h"
#include "utils.h"

#include "open.h"
#include "close.h"

void changeDirectory(string dir_name)
{
    char *dir[10];
    int fd;
    int i = 0, j = 0;
    //将输入按/分割开
    dir[0] = strtok((char *)dir_name.data(), "/");
    while (true)
    {
        i++;
        dir[i] = strtok(nullptr, "/");
        if (!dir[i])
        {
            break;
        }
    }
    if (strcmp(dir[0], ".") == 0)
    { //当前目录
        j++;
    }
    else if (strcmp(dir[0], "..") == 0)
    { //上级目录
        if (CURRENT_DIR)
        {
            CURRENT_DIR = close(CURRENT_DIR);
        }
        j++;
    }
    else if (strcmp(dir[0], "root") == 0)
    { //返回根目录
        while (CURRENT_DIR)
        {
            CURRENT_DIR = close(CURRENT_DIR);
        }
        j++;
    }
    //向下搜索找到要打开的文件夹
    for (; j < i; j++)
    {
        fd = open(dir[j]);
        if (fd != -1)
        {
            //设置当前目录为该目录
            CURRENT_DIR = fd;
        }
        else
        {
            return;
        }
    }
}