#include "new.h"

void createFileSystem(const string &filename)
{
    cout << "begin to create the file system" << endl;
    //虚拟磁盘
    VIRTUALDISK = (unsigned char *)malloc(DISK_SIZE);
    memset(VIRTUALDISK, 0, DISK_SIZE);
    //虚拟磁盘第一个块作为引导块
    BOOT *BOOT_BLOCK;
    BOOT_BLOCK = (BOOT *)VIRTUALDISK;
    //两张FAT表
    FAT *FAT1, *FAT2;
    FAT1 = (FAT *)(VIRTUALDISK + BLOCK_SIZE);
    FAT2 = (FAT *)(VIRTUALDISK + 3 * BLOCK_SIZE);
    strcpy(BOOT_BLOCK->flag, "20000521");
    strcpy(BOOT_BLOCK->information, "FileSystem Designed by FanHongtao");
    for (int i = 0; i < 5; i++)
    {
        FAT1->id = END;
        FAT2->id = END;
        FAT1++;
        FAT2++;
    }
    FAT1->id = 6;
    FAT2->id = 6;
    FAT1++;
    FAT2++;
    FAT1->id = END;
    FAT2->id = END;
    FAT1++;
    FAT2++;
    for (int i = 7; i < DISK_SIZE / BLOCK_SIZE; i++)
    {
        FAT1->id = FREE;
        FAT2->id = FREE;
        FAT1++;
        FAT2++;
    }

    //在该磁盘上创建目录项.和..
    time_t now;
    tm *nowTime;

    FCB *root;
    root = (FCB *)(VIRTUALDISK + 5 * BLOCK_SIZE);

    now = time(nullptr);
    nowTime = localtime(&now);
    strcpy(root->filename, ".");
    root->attribute = VIRTUAL_TYPE;
    root->time = nowTime->tm_hour * 2048 + nowTime->tm_min * 32 + nowTime->tm_sec / 2;
    root->date = (nowTime->tm_year) * 512 + (nowTime->tm_mon + 1) * 32 + nowTime->tm_mday;
    root->first = 5;
    root->length = 2 * sizeof(FCB);
    root->free = true;
    root++;

    now = time(nullptr);
    nowTime = localtime(&now);
    strcpy(root->filename, "..");
    root->attribute = VIRTUAL_TYPE;
    root->time = nowTime->tm_hour * 2048 + nowTime->tm_min * 32 + nowTime->tm_sec / 2;
    root->date = (nowTime->tm_year - 80) * 512 + (nowTime->tm_mon + 1) * 32 + nowTime->tm_mday;
    root->first = 5;
    root->length = 2 * sizeof(FCB);
    root->free = true;

    //将虚拟磁盘保存为文件
    FILE *file;
    file = fopen(filename.c_str(), "w");
    fwrite(VIRTUALDISK, DISK_SIZE, 1, file);
    fclose(file);

    cout << "the file system created success" << endl;
}
