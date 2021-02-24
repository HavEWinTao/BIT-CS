#include "read.h"
#include "utils.h"

//定义一个字符型数组text[len]，用来接收用户从文件中读出的文件内容；

//调用readDisk()将指定文件中的len字节内容读出到text[]中；
//如果readDisk()的返回值为负，则显示出错信息；否则将text[]中的内容显示到屏幕上；
int read(int fd, unsigned int len)
{
    //判断文件是否存在
    if (fd < 0 || fd >= MAX_OPEN_FILE)
    {
        cout << "Error: the file is not exist" << endl;
        return -1;
    }
    //判断文件是否为空
    if (OPEN_FILE_LIST[fd].fcb.length == 0)
    {
        cout << "Error: the file is empty" << endl;
        return -1;
    }
    //用来接收用户从文件中读出的文件内容
    char text[MAX_TEXT];
    int temp;
    OPEN_FILE_LIST[fd].count = 0;
    temp = readDisk(fd, len, text);
    //输出读取内容
    if (temp != -1)
    {
        cout << text;
        return temp;
    }
    else
    {
        cout << "Error: read failed" << endl;
        return -1;
    }
}
