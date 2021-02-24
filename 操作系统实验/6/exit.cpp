#include "exit.h"
#include "utils.h"

int closeFile(int fd)
{
    //检查fd的有效性
    if (fd < 0 || fd >= MAX_OPEN_FILE)
    {
        printf("Error: The file is not exist.\n");
        return -1;
    }
    FCB *fcb;
    int father;
    father = OPEN_FILE_LIST[fd].father;
    //检查用户打开文件表表项中的fcbstate字段的值
    if (OPEN_FILE_LIST[fd].fcbstate)
    {
        //将该文件的FCB的内容保存到虚拟磁盘上该文件的目录项中
        fcb = (FCB *)malloc(sizeof(FCB));
        strcpy(fcb->filename, OPEN_FILE_LIST[fd].fcb.filename);
        fcb->attribute = OPEN_FILE_LIST[fd].fcb.attribute;
        fcb->time = OPEN_FILE_LIST[fd].fcb.time;
        fcb->date = OPEN_FILE_LIST[fd].fcb.date;
        fcb->first = OPEN_FILE_LIST[fd].fcb.first;
        fcb->length = OPEN_FILE_LIST[fd].fcb.length;
        fcb->free = OPEN_FILE_LIST[fd].fcb.free;
        //打开该文件的父目录文件
        father = OPEN_FILE_LIST[fd].father;
        OPEN_FILE_LIST[father].count = OPEN_FILE_LIST[fd].diroff * sizeof(FCB);
        //将欲关闭文件的FCB写入父目录文件的相应盘块中
        writeDisk(father, (char *)fcb, sizeof(FCB));
        free(fcb);
        OPEN_FILE_LIST[fd].fcbstate = 0;
    }
    //回收该文件占据的用户打开文件表表项,将free字段置为false
    strcpy(OPEN_FILE_LIST[fd].fcb.filename, "");
    OPEN_FILE_LIST[fd].free = false;
    return father;
}

void exitSystem(string filename)
{
    while (CURRENT_DIR)
    {
        CURRENT_DIR = closeFile(CURRENT_DIR);
    }
    //使用fopen()打开磁盘上的文件
    FILE *file;
    file = fopen(filename.c_str(), "w");
    //将虚拟磁盘空间中的所有内容保存到磁盘上的文件中
    fwrite(VIRTUALDISK, DISK_SIZE, 1, file);
    //使用fclose()关闭文件
    fclose(file);
    //撤销用户打开文件表，释放其内存空间,释放虚拟磁盘空间
    free(VIRTUALDISK);
    cout << "exit file system successfully" << endl;
}