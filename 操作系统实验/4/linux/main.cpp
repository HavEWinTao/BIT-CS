#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <utime.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#define MAX_PATH 260
#define BUF_SIZE 1024

using namespace std;

//打印文件树型目录
void PrintFilename(char *name, int depth)
{
    for (int i = 0; i < depth * 4; i++)
    {
        cout << " ";
    }
    cout << name << endl;
}

//复制文件
void CopyFile(char src[MAX_PATH], char dest[MAX_PATH], int depth)
{
    PrintFilename(src, depth); //打印文件名
    char linkname[MAX_PATH];
    int judge_softlink = readlink(src, linkname, MAX_PATH - 1);
    //软连接
    if (judge_softlink != -1)
    {
        linkname[judge_softlink] = '\0';
        symlink(linkname,dest);
    }
    //文件
    else
    {
        int srcFile = open(src, O_RDONLY, S_IRUSR);

        struct stat Stat;
        struct utimbuf Time;

        //复制过程中的缓冲区
        char *buffer = new char[BUF_SIZE];

        //读和写的字节数
        int size;

        stat(src, &Stat);

        int destFile = creat(dest, Stat.st_mode);

        //以缓冲区大小复制
        //读文件
        while ((size = read(srcFile, buffer, BUF_SIZE)) > 0)
        {
            //写文件
            write(destFile, buffer, size);
        }

        // 记录最后访问时间和最后修改时间
        Time.actime = Stat.st_atime;
        Time.modtime = Stat.st_mtime;

        //关闭文件
        close(srcFile);
        close(destFile);

        utime(dest, &Time);
    }
}

//复制文件夹
void CopyDirectory(char src[MAX_PATH], char dest[MAX_PATH], int depth)
{
    //存储源路径和目标路径
    char source[MAX_PATH];
    char target[MAX_PATH];

    strcpy(source, src);
    strcpy(target, dest);

    DIR *dir;
    dir = opendir(source);

    struct stat Stat;
    struct utimbuf Time;

    struct dirent *entry;

    while (entry = readdir(dir))
    {
        //防止多文件夹错误,请理缓存
        //当前文件源路径
        strcpy(source, src);
        strcat(source, "/");
        strcat(source, entry->d_name);
        //当前文件目标路径
        strcpy(target, dest);
        strcat(target, "/");
        strcat(target, entry->d_name);

        //只复制当前文件夹下的子文件夹
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }
        //复制文件夹
        if (entry->d_type == DT_DIR)
        {
            PrintFilename(entry->d_name, depth);
            //创建文件夹并复制
            // 取得指定文件的文件属性
            stat(src, &Stat);
            // 建立目标文件目录
            mkdir(target, Stat.st_mode);
            // 记录最后访问时间和最后修改时间
            Time.actime = Stat.st_atime;
            Time.modtime = Stat.st_mtime;
            //更改文件的访问和修改时间
            utime(dest, &Time);
            //递归复制文件夹
            CopyDirectory(source, target, depth + 1);
        }
        //复制文件
        else
        {
            //复制文件
            CopyFile(source, target, depth);
        }
    }
}

int main(int argc, char *argv[])
{
    // 参数判断
    if (argc != 3)
    {
        cout << "Error:input as"
             << " src"
             << " dest" << endl;
        return -1;
    }

    char src[MAX_PATH];
    char dest[MAX_PATH];

    // 第一个参数：源文件夹
    sprintf(src, "%s", argv[1]);
    // 第二个参数：目标文件夹
    sprintf(dest, "%s", argv[2]);

    DIR *dir;

    struct stat Stat;
    struct utimbuf Time;

    // 取得指定文件的文件属性
    stat(src, &Stat);

    if (S_ISDIR(Stat.st_mode))
    {
        dir = opendir(src);

        // 如果dest文件夹不存在则创建目标文件夹
        if ((dir = opendir(dest)) == NULL)
        {

            // 建立目标文件目录
            mkdir(dest, Stat.st_mode);
            // 记录最后访问时间和最后修改时间
            Time.actime = Stat.st_atime;
            Time.modtime = Stat.st_mtime;
            //更改文件的访问和修改时间
            utime(dest, &Time);
        }

        cout << "-----copying file list-----" << endl
             << endl;
        cout << src << endl;
        //递归复制文件夹
        CopyDirectory(src, dest, 1);
    }
    //判断拷贝的是文件
    else
    {
        cout << "-----copying file-----" << endl
             << endl;
        CopyFile(src, dest, 0);
    }
    //拷贝完成
    cout << endl
         << "-----copy finished-----" << endl;

    return 0;
}
