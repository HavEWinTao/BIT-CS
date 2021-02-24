#include "delete.h"
#include "utils.h"

void deleteFile(string file_name)
{
    //检查文件名
    const char *fname;
    fname = file_name.c_str();
    if (strcmp(fname, "") == 0)
    {
        cout << "Error: please input a file name" << endl;
        return;
    }
    //调用readDisk()读出该父目录文件内容到内存，检查该目录下欲删除文件是否存在
    char text[MAX_TEXT];
    int rbn, i;
    OPEN_FILE_LIST[CURRENT_DIR].count = 0;
    rbn = readDisk(CURRENT_DIR, OPEN_FILE_LIST[CURRENT_DIR].fcb.length, text);

    FCB *fcb;
    fcb = (FCB *)text;
    for (i = 0; i < rbn / sizeof(FCB); i++)
    {
        if (strcmp(fcb->filename, fname) == 0)
        {
            break;
        }
        fcb++;
    }
    if (fcb->attribute != FILE_TYPE)
    {
        cout << "delete command can only delete file,please use rmdir to delete a directory" << endl;
        return;
    }
    if (i == rbn / sizeof(FCB))
    {
        cout << "Error: the file is not exist" << endl;
        return;
    }
    //定位FAT
    FAT *FAT1, *FAT2;
    FAT1 = (FAT *)(VIRTUALDISK + BLOCK_SIZE);
    FAT2 = (FAT *)(VIRTUALDISK + 3 * BLOCK_SIZE);
    //回收该文件所占据的磁盘块，修改FAT
    FAT *fat1, *fat2;
    unsigned short block_index;
    block_index = fcb->first;
    while (block_index != END)
    {
        fat1 = FAT1 + block_index;
        fat2 = FAT2 + block_index;
        block_index = fat1->id;
        fat1->id = FREE;
        fat2->id = FREE;
    }
    strcpy(fcb->filename, "");
    //从文件的目录文件中清空该文件的目录项，且free字段置为0
    fcb->free = false;
    //修改该目录文件的用户打开文件表项中的长度信息
    OPEN_FILE_LIST[CURRENT_DIR].count = i * sizeof(FCB);
    writeDisk(CURRENT_DIR, (char *)fcb, sizeof(FCB));
    //将该表项中的fcbstate置为1
    OPEN_FILE_LIST[CURRENT_DIR].fcbstate = 1;

    cout << "the file " << fname << " is deleted successfully" << endl;
}
