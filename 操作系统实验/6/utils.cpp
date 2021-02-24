#include "utils.h"

unsigned char *VIRTUALDISK;

OPENED_FILE OPEN_FILE_LIST[MAX_OPEN_FILE];

int CURRENT_DIR;

int findBlock()
{
    unsigned short index;
    FAT *FAT1, *fat;
    //定位FAT表
    FAT1 = (FAT *)(VIRTUALDISK + BLOCK_SIZE);
    for (index = 7; index < DISK_SIZE / BLOCK_SIZE; index++)
    {
        fat = FAT1 + index;
        //空闲块
        if (fat->id == FREE)
        {
            return index;
        }
    }
    cout << "Error: can't find free block" << endl;
    return -1;
}

int writeDisk(int fd, const char *text, int len)
{
    //定位FAT
    FAT *FAT1, *FAT2;
    FAT1 = (FAT *)(VIRTUALDISK + BLOCK_SIZE);
    FAT2 = (FAT *)(VIRTUALDISK + 3 * BLOCK_SIZE);
    //buffer缓冲区
    unsigned char *buf, *block;
    buf = (unsigned char *)malloc(BLOCK_SIZE);
    if (buf == nullptr)
    {
        cout << "malloc failed" << endl;
        return -1;
    }
    //块索引和块内位移
    unsigned short block_index, block_off;
    block_index = OPEN_FILE_LIST[fd].fcb.first;
    block_off = OPEN_FILE_LIST[fd].count;
    //定位当前文件所在块
    FAT *fat1, *fat2;
    fat1 = FAT1 + block_index;
    fat2 = FAT2 + block_index;
    //定位当前指针位置
    while (block_off >= BLOCK_SIZE)
    {
        block_index = fat1->id;
        if (block_index == END)
        {
            //找到下一个空闲块
            block_index = findBlock();
            if (block_index == -1)
            {
                free(buf);
                return -1;
            }
            fat1->id = block_index;
            fat2->id = block_index;
            fat1 = FAT1 + block_index;
            fat2 = FAT2 + block_index;
            fat1->id = END;
            fat2->id = END;
        }
        else
        {
            fat1 = FAT1 + block_index;
            fat2 = FAT2 + block_index;
        }
        block_off -= BLOCK_SIZE;
    }
    //写入数据
    int temp = 0;
    while (temp < len)
    {
        block = VIRTUALDISK + block_index * BLOCK_SIZE;
        //带有偏移的写入需要先将数据复制到缓冲区
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            buf[i] = block[i];
        }
        for (; block_off < BLOCK_SIZE; block_off++)
        {
            buf[block_off] = text[temp++];
            OPEN_FILE_LIST[fd].count++;
            if (temp == len)
            {
                break;
            }
        }
        //逐个字节写入数据
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            block[i] = buf[i];
        }
        if (temp < len)
        {
            //存入下一个数据块
            block_index = fat1->id;
            if (block_index == END)
            {
                block_index = findBlock();
                if (block_index == -1)
                {
                    break;
                }
                fat1->id = block_index;
                fat2->id = block_index;
                fat1 = FAT1 + block_index;
                fat2 = FAT2 + block_index;
                fat1->id = END;
                fat2->id = END;
            }
            else
            {
                fat1 = FAT1 + block_index;
                fat2 = FAT2 + block_index;
            }
            //将指针定位为下个块的开始位置
            block_off = 0;
        }
    }
    //更改指针位置
    if (OPEN_FILE_LIST[fd].count > OPEN_FILE_LIST[fd].fcb.length)
    {
        OPEN_FILE_LIST[fd].fcb.length = OPEN_FILE_LIST[fd].count;
    }
    OPEN_FILE_LIST[fd].fcbstate = 1;
    //释放缓冲区
    free(buf);
    //返回写入长度
    return temp;
}

int readDisk(int fd, unsigned int len, char *text)
{
    //定位FAT
    FAT *FAT1;
    FAT1 = (FAT *)(VIRTUALDISK + BLOCK_SIZE);
    //buffer缓冲区
    unsigned char *buf, *block;
    buf = (unsigned char *)malloc(BLOCK_SIZE);
    if (buf == nullptr)
    {
        cout << "Error: malloc failed" << endl;
        return -1;
    }
    //块索引和块内位移
    unsigned short block_index, block_off;
    block_index = OPEN_FILE_LIST[fd].fcb.first;
    block_off = OPEN_FILE_LIST[fd].count;
    if (block_off >= OPEN_FILE_LIST[fd].fcb.length)
    {
        cout << "Error: read out of range" << endl;
        free(buf);
        return -1;
    }
    //定位当前文件所在块
    FAT *fat;
    fat = FAT1 + block_index;
    while (block_off >= BLOCK_SIZE)
    {
        block_index = fat->id;
        block_off -= BLOCK_SIZE;
        fat = FAT1 + block_index;
    }
    //写入数据
    int temp = 0;
    while (temp < len)
    {
        block = VIRTUALDISK + block_index * BLOCK_SIZE;
        //将内容写入buffer
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            buf[i] = block[i];
        }
        //带有偏移量
        for (; block_off < BLOCK_SIZE; block_off++)
        {
            text[temp++] = buf[block_off];
            OPEN_FILE_LIST[fd].count++;
            if (temp == len || OPEN_FILE_LIST[fd].count == OPEN_FILE_LIST[fd].fcb.length)
            {
                break;
            }
        }
        //下一个文件所在块
        if (temp < len && OPEN_FILE_LIST[fd].count != OPEN_FILE_LIST[fd].fcb.length)
        {
            block_index = fat->id;
            if (block_index == END)
            {
                break;
            }
            block_off = 0;
            fat = FAT1 + block_index;
        }
    }
    //字符串结束符
    text[temp] = '\0';
    //释放缓冲区
    free(buf);
    //返回读取长度
    return temp;
}

int findOpenFile()
{
    //找到打开的文件
    for (int i = 0; i < MAX_OPEN_FILE; i++)
    {
        if (!OPEN_FILE_LIST[i].free)
        {
            return i;
        }
    }
    cout << "Error: Open too many files" << endl;
    return -1;
}
