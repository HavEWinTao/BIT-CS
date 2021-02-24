#include "close.h"
#include "utils.h"

int close(int fd)
{
    FCB *fcb;
    int father;
    //检查fd的有效性,文件不存在
    if (fd < 0 || fd >= MAX_OPEN_FILE)
    {
        cout << "Error: the file is not exist" << endl;
        return -1;
    }
    father = OPEN_FILE_LIST[fd].father;
    //检查用户打开文件表表项中的fcbstate字段的值
    //如果为1则需要将该文件的FCB的内容保存到虚拟磁盘上该文件的目录项中
    if (OPEN_FILE_LIST[fd].fcbstate)
    {
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
        //调用writeDisk()将欲关闭文件的FCB写入父目录文件的相应盘块中；
        writeDisk(father, (char *)fcb, sizeof(FCB));
        free(fcb);
        OPEN_FILE_LIST[fd].fcbstate = 0;
    }
    //回收该文件占据的用户打开文件表表项,将free字段置为false
    strcpy(OPEN_FILE_LIST[fd].fcb.filename, "");
    OPEN_FILE_LIST[fd].free = false;
    //返回父目录
    return father;
}
