#include "create.h"
#include "utils.h"

void createFile(string file_name)
{
    //新建文件名
    const char *fname;
    fname = file_name.c_str();
    if (strcmp(fname, "") == 0)
    {
        cout << "Error: file must have a right name" << endl;
        return;
    }
    int rbn, i;
    //当前目录位置
    char text[MAX_TEXT];
    OPEN_FILE_LIST[CURRENT_DIR].count = 0;
    rbn = readDisk(CURRENT_DIR, OPEN_FILE_LIST[CURRENT_DIR].fcb.length, text);
    //判断文件是否已经存在
    FCB *fcb;
    fcb = (FCB *)text;
    for (i = 0; i < rbn / sizeof(FCB); i++)
    {
        if (strcmp(fcb->filename, fname) == 0)
        {
            cout << "Error: this file/directory is already exist" << endl;
            return;
        }
        fcb++;
    }
    //为新文件分配FCB
    fcb = (FCB *)text;
    for (i = 0; i < rbn / sizeof(FCB); i++)
    {
        if (fcb->free == 0)
        {
            break;
        }
        fcb++;
    }
    //找空闲块
    unsigned short block_index;
    block_index = findBlock();
    if (block_index == -1)
    {
        cout << "Error: the disk is already full" << endl;
        return;
    }
    //定位FAT
    FAT *FAT1, *FAT2;
    FAT1 = (FAT *)(VIRTUALDISK + BLOCK_SIZE);
    FAT2 = (FAT *)(VIRTUALDISK + 3 * BLOCK_SIZE);
    (FAT1 + block_index)->id = END;
    (FAT2 + block_index)->id = END;
    //设置时间等其他属性
    time_t now;
    tm *nowTime;
    now = time(nullptr);
    nowTime = localtime(&now);
    strcpy(fcb->filename, fname);
    fcb->attribute = FILE_TYPE;
    fcb->time = nowTime->tm_hour * 2048 + nowTime->tm_min * 32 + nowTime->tm_sec / 2;
    fcb->date = (nowTime->tm_year) * 512 + (nowTime->tm_mon + 1) * 32 + nowTime->tm_mday;
    fcb->first = block_index;
    fcb->length = 0;
    fcb->free = true;
    //修改当前目录和创建文件
    OPEN_FILE_LIST[CURRENT_DIR].count = i * sizeof(FCB);
    writeDisk(CURRENT_DIR, (char *)fcb, sizeof(FCB));
    fcb = (FCB *)text;
    fcb->length = OPEN_FILE_LIST[CURRENT_DIR].fcb.length;
    OPEN_FILE_LIST[CURRENT_DIR].count = 0;
    writeDisk(CURRENT_DIR, (char *)fcb, sizeof(FCB));
    OPEN_FILE_LIST[CURRENT_DIR].fcbstate = 1;

    cout << "the file " << file_name << " is created successfully" << endl;
}
