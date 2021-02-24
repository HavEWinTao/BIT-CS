#include "open.h"
#include "utils.h"

int open(string filename)
{
    //检查该文件是否已经打开
    const char *fname;
    int i;
    fname = filename.c_str();
    for (i = 0; i < MAX_OPEN_FILE; i++)
    {
        if (strcmp(OPEN_FILE_LIST[i].fcb.filename, fname) == 0 && i != CURRENT_DIR)
        {
            cout << "Error: the file is already open." << endl;
            return -1;
        }
    }
    //检查该目录下欲打开文件是否存在
    int rbn;
    FCB *fcbptr;
    char text[MAX_TEXT];
    OPEN_FILE_LIST[CURRENT_DIR].count = 0;
    rbn = readDisk(CURRENT_DIR, OPEN_FILE_LIST[CURRENT_DIR].fcb.length, text);
    fcbptr = (FCB *)text;
    for (i = 0; i < rbn / sizeof(FCB); i++)
    {
        if (strcmp(fcbptr->filename, fname) == 0)
            break;
        fcbptr++;
    }
    if (i == rbn / sizeof(FCB))
    {
        cout << "Error: the file is not exist." << endl;
        return -1;
    }
    //检查用户打开文件表中是否有空表项
    int fd;
    fd = findOpenFile();
    if (fd == -1)
    {
        return -1;
    }
    //为该文件填写空白用户打开文件表表项内容,读写指针置为0
    strcpy(OPEN_FILE_LIST[fd].fcb.filename, fcbptr->filename);
    OPEN_FILE_LIST[fd].fcb.attribute = fcbptr->attribute;
    OPEN_FILE_LIST[fd].fcb.time = fcbptr->time;
    OPEN_FILE_LIST[fd].fcb.date = fcbptr->date;
    OPEN_FILE_LIST[fd].fcb.first = fcbptr->first;
    OPEN_FILE_LIST[fd].fcb.length = fcbptr->length;
    OPEN_FILE_LIST[fd].fcb.free = fcbptr->free;
    OPEN_FILE_LIST[fd].diroff = i;
    strcpy(OPEN_FILE_LIST[fd].dir, OPEN_FILE_LIST[CURRENT_DIR].dir);
    strcat(OPEN_FILE_LIST[fd].dir, filename.c_str());
    if (fcbptr->attribute != FILE_TYPE)
    {
        strcat(OPEN_FILE_LIST[fd].dir, "\\");
    }
    OPEN_FILE_LIST[fd].father = CURRENT_DIR;
    OPEN_FILE_LIST[fd].count = 0;
    OPEN_FILE_LIST[fd].fcbstate = 0;
    OPEN_FILE_LIST[fd].free = false;
    //将该文件所分配到的空白用户打开文件表表项序号作为文件描述符fd返回
    return fd;
}
