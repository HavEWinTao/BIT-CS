#include "sfs.h"

bool openFileSystem(string filename)
{
    FILE *file;
    unsigned char BUFFER[DISK_SIZE];
    //将磁盘内容读取到内存
    if ((file = fopen(filename.c_str(), "r")) != nullptr)
    {
        fread(BUFFER, DISK_SIZE, 1, file);
        if (strcmp(((BOOT *)BUFFER)->flag, "20000521") != 0)
        {
            VIRTUALDISK = (unsigned char *)malloc(DISK_SIZE);
            memset(VIRTUALDISK, 0, DISK_SIZE);
            for (int i = 0; i < DISK_SIZE; i++)
            {
                VIRTUALDISK[i] = BUFFER[i];
            }
        }
        else
        { //磁盘损坏
            fclose(file);
            cout << "the virtual disk is destroyed" << endl;
            return false;
        }
    }
    else
    { //磁盘不存在
        cout << "the virtual disk is not exist" << endl;
        return false;
    }
    //读取磁盘根目录
    FCB *root;
    root = (FCB *)(VIRTUALDISK + 5 * BLOCK_SIZE);
    strcpy(OPEN_FILE_LIST[0].fcb.filename, root->filename);
    OPEN_FILE_LIST[0].fcb.attribute = root->attribute;
    OPEN_FILE_LIST[0].fcb.time = root->time;
    OPEN_FILE_LIST[0].fcb.date = root->date;
    OPEN_FILE_LIST[0].fcb.first = root->first;
    OPEN_FILE_LIST[0].fcb.length = root->length;
    OPEN_FILE_LIST[0].fcb.free = root->free;
    OPEN_FILE_LIST[0].diroff = 0;
    strcpy(OPEN_FILE_LIST[0].dir, "disk:\\");
    OPEN_FILE_LIST[0].father = 0;
    OPEN_FILE_LIST[0].count = 0;
    OPEN_FILE_LIST[0].fcbstate = 0;
    OPEN_FILE_LIST[0].free = true;
    for (int i = 1; i < MAX_OPEN_FILE; i++)
    {
        OPEN_FILE_LIST[i].free = false;
    }
    CURRENT_DIR = 0;

    cout << "the virtual disk is loaded successfully" << endl;
    return true;
}
