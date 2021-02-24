#include "rmdir.h"
#include "utils.h"

#include "open.h"
#include "close.h"

void deleteDirectory(string dir_name)
{
    //检查文件夹是否合法
    if (strcmp(dir_name.c_str(), ".") == 0 || strcmp(dir_name.c_str(), "..") == 0)
    {
        cout << "Error: can't remove this directory." << endl;
        return;
    }
    //检查当前目录下欲删除目录文件夹是否存在
    char text1[MAX_TEXT], text2[MAX_TEXT];
    int rbn1, rbn2, fd, i, j;
    OPEN_FILE_LIST[CURRENT_DIR].count = 0;
    rbn1 = readDisk(CURRENT_DIR, OPEN_FILE_LIST[CURRENT_DIR].fcb.length, text1);
    FCB *fcb1, *fcb2;
    fcb1 = (FCB *)text1;
    for (i = 0; i < rbn1 / sizeof(FCB); i++)
    {
        if (strcmp(fcb1->filename, dir_name.c_str()) == 0)
        {
            break;
        }
        fcb1++;
    }
    if (i == rbn1 / sizeof(FCB))
    {
        cout << "Error: the directory is not exist" << endl;
        return;
    }
    //判断删除的是否为文件夹
    if (fcb1->attribute != FOLDER_TYPE)
    {
        cout << "Error: rmdir command can only delete a directory,please use delete to delete a file" << endl;
        return;
    }
    //判断文件夹是否为空
    fd = open(dir_name);
    rbn2 = readDisk(fd, OPEN_FILE_LIST[fd].fcb.length, text2);
    fcb2 = (FCB *)text2;
    for (j = 0; j < rbn2 / sizeof(FCB); j++)
    {
        if (strcmp(fcb2->filename, ".") != 0 && strcmp(fcb2->filename, "..") != 0 && strcmp(fcb2->filename, "") != 0)
        {
            close(fd);
            cout << "Error: the directory is not empty" << endl;
            return;
        }
        fcb2++;
    }
    //回收该目录文件所占据的磁盘块,修改FAT
    FAT *FAT1, *FAT2;
    FAT1 = (FAT *)(VIRTUALDISK + BLOCK_SIZE);
    FAT2 = (FAT *)(VIRTUALDISK + 3 * BLOCK_SIZE);
    FAT *fat1, *fat2;
    unsigned short block_index;
    block_index = OPEN_FILE_LIST[fd].fcb.first;
    while (block_index != END)
    {
        fat1 = FAT1 + block_index;
        fat2 = FAT2 + block_index;
        block_index = fat1->id;
        fat1->id = FREE;
        fat2->id = FREE;
    }
    close(fd);
    //从当前目录中清空该目录的目录项,且free字段置为false
    strcpy(fcb1->filename, "");
    fcb1->free = false;
    //修改当前目录文件的用户打开表项中的长度信息,并将表项中的fcbstate置为1
    OPEN_FILE_LIST[CURRENT_DIR].count = i * sizeof(FCB);
    writeDisk(CURRENT_DIR, (char *)fcb1, sizeof(FCB));
    OPEN_FILE_LIST[CURRENT_DIR].fcbstate = 1;

    cout << "delete directory " << dir_name << " successfully" << endl;
}
