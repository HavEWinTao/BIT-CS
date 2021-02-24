#include "mkdir.h"
#include "utils.h"

#include "open.h"
#include "close.h"

void makeDirectory(string dir_name)
{
    //检查当前目录下新建目录文件是否重名
    int rbn, fd;
    char text[MAX_TEXT];
    OPEN_FILE_LIST[CURRENT_DIR].count = 0;
    rbn = readDisk(CURRENT_DIR, OPEN_FILE_LIST[CURRENT_DIR].fcb.length, text);
    FCB *fcb;
    fcb = (FCB *)text;
    for (int i = 0; i < rbn / sizeof(FCB); i++)
    {
        if (strcmp(fcb->filename, dir_name.c_str()) == 0)
        {
            cout << "ERROR: this directory/file is already exist" << endl;
            return;
        }
        fcb++;
    }
    //分配一个空闲文件表项
    fcb = (FCB *)text;
    int count;
    for (count = 0; count < rbn / sizeof(FCB); count++)
    {
        if (!fcb->free)
        {
            break;
        }
        fcb++;
    }
    //检查FAT是否有空闲的盘块
    unsigned short block_index;
    block_index = findBlock();
    if (block_index == -1)
    {
        return;
    }
    //定位FAT
    FAT *FAT1, *FAT2;
    FAT1 = (FAT *)(VIRTUALDISK + BLOCK_SIZE);
    FAT2 = (FAT *)(VIRTUALDISK + 3 * BLOCK_SIZE);
    (FAT1 + block_index)->id = END;
    (FAT2 + block_index)->id = END;
    //新建目录文件的FCB的内容
    time_t now;
    tm *nowTime;
    now = time(nullptr);
    nowTime = localtime(&now);
    strcpy(fcb->filename, dir_name.c_str());
    fcb->attribute = FOLDER_TYPE;
    fcb->time = nowTime->tm_hour * 2048 + nowTime->tm_min * 32 + nowTime->tm_sec / 2;
    fcb->date = (nowTime->tm_year) * 512 + (nowTime->tm_mon + 1) * 32 + nowTime->tm_mday;
    fcb->first = block_index;
    fcb->length = 2 * sizeof(FCB);
    fcb->free = true;
    OPEN_FILE_LIST[CURRENT_DIR].count = count * sizeof(FCB);
    writeDisk(CURRENT_DIR, (char *)fcb, sizeof(FCB));
    fd = open(dir_name);
    if (fd == -1)
    {
        return;
    }
    //在新建目录文件所分配到的磁盘块中建立两个特殊的目录项.和..目录项
    fcb = (FCB *)malloc(sizeof(FCB));
    now = time(nullptr);
    nowTime = localtime(&now);
    strcpy(fcb->filename, ".");
    fcb->attribute = VIRTUAL_TYPE;
    fcb->time = nowTime->tm_hour * 2048 + nowTime->tm_min * 32 + nowTime->tm_sec / 2;
    fcb->date = (nowTime->tm_year - 80) * 512 + (nowTime->tm_mon + 1) * 32 + nowTime->tm_mday;
    fcb->first = block_index;
    fcb->length = 2 * sizeof(FCB);
    fcb->free = true;
    writeDisk(fd, (char *)fcb, sizeof(FCB));

    now = time(nullptr);
    nowTime = localtime(&now);
    strcpy(fcb->filename, "..");
    fcb->attribute = VIRTUAL_TYPE;
    fcb->time = nowTime->tm_hour * 2048 + nowTime->tm_min * 32 + nowTime->tm_sec / 2;
    fcb->date = (nowTime->tm_year - 80) * 512 + (nowTime->tm_mon + 1) * 32 + nowTime->tm_mday;
    fcb->first = block_index;
    fcb->length = 2 * sizeof(FCB);
    fcb->free = true;
    writeDisk(fd, (char *)fcb, sizeof(FCB));

    free(fcb);
    close(fd);
    //修改当前目录文件的长度信息，并将当前目录文件的用户打开文件表项中的fcbstate置为1
    fcb = (FCB *)text;
    fcb->length = OPEN_FILE_LIST[CURRENT_DIR].fcb.length;
    OPEN_FILE_LIST[CURRENT_DIR].count = 0;
    writeDisk(CURRENT_DIR, (char *)fcb, sizeof(FCB));
    OPEN_FILE_LIST[CURRENT_DIR].fcbstate = 1;

    cout << "the directory " << dir_name << " is created successfully" << endl;
}
