#ifndef FILESYSTEM_UTILS_H
#define FILESYSTEM_UTILS_H

#include <iostream>

using namespace std;

// 查找空闲块
int findBlock();

//模拟读磁盘
int readDisk(int fd, unsigned int len, char *text);

//模拟写磁盘
int writeDisk(int fd, const char *text, int len);

//找打开文件
int findOpenFile();

//类型 1字节
enum FileType : unsigned char
{
    FOLDER_TYPE, //文件夹
    FILE_TYPE,   //文件
    VIRTUAL_TYPE //.,..
};

//文件控制块32字节
struct FCB
{
    char filename[11];    //文件名
    FileType attribute;   //类型
    unsigned short time;  //时间
    unsigned short date;  //日期
    unsigned short first; //首个文件块
    unsigned long length; //文件长度
    bool free;            //是否被占用
};

//虚拟磁盘
//|  0     Boot
//|  1-2   FAT1
//|  3-4   FAT2
//|  6-999 Data Block
//|  5     ROOT_DIR
extern unsigned char *VIRTUALDISK;

//磁盘大小
const int DISK_SIZE = 1000 * 1024;
//块大小
const int BLOCK_SIZE = 1024;

//FAT中文件结束和空闲标识
const int END = 65535;
const int FREE = 0;

//链接结构
struct FAT
{
    unsigned short id; //下一个数据块的索引
};

//引导块
struct BOOT
{
    char flag[8];          //自检标识
    char information[128]; //磁盘的描述信息
};

//最长路径
const int MAX_PATH_LEN = 80;

//用户打开文件
struct OPENED_FILE
{
    FCB fcb;                //文件控制块
    int diroff;             //偏移量
    char dir[MAX_PATH_LEN]; //打开文件的路径名
    int father;             //父目录所在打开文件表的位置
    unsigned short count;   //读写指针在文件中的位置
    char fcbstate;          //是否修改了FCB的内容
    bool free;              //用户打开表项是否为空
};

//最大打开文件数
const int MAX_OPEN_FILE = 10;
//打开文件表
extern OPENED_FILE OPEN_FILE_LIST[MAX_OPEN_FILE];

//当前文件目录
extern int CURRENT_DIR;

//每行最大字节数
const int MAX_TEXT = 10000;

#endif //FILESYSTEM_UTILS_H
