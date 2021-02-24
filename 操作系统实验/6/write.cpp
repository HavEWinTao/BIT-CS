#include "write.h"
#include "utils.h"

int write(int fd)
{
    //检查fd的有效性
    if (fd < 0 || fd >= MAX_OPEN_FILE)
    {
        cout << "Error: the file is not exist" << endl;
        return -1;
    }
    int wstyle;
    //提示并等待用户输入写方式
    cout << "input write style:" << endl;
    cout << "1.cover write      2.add write" << endl;
    while (true)
    {

        scanf("%d", &wstyle);
        if (wstyle > 0 && wstyle < 3)
        {
            break;
        }
        cout << "Error: input error,please input again" << endl;
    }
    getchar();
    if (wstyle == 1)
    {
        //覆盖写
        OPEN_FILE_LIST[fd].count = 0;
    }
    else if (wstyle == 2)
    {
        //修改文件的当前读写指针位置到文件的末尾
        OPEN_FILE_LIST[fd].count = OPEN_FILE_LIST[fd].fcb.length;
    }
    //将用户的本次输入内容保存到一临时变量text[]中
    char text[MAX_TEXT];
    int len, all_len, temp;
    all_len = 0;
    cout << "input data(end with EOF):" << endl;
    while (gets(text) != nullptr)
    {
        len = strlen(text);
        text[len++] = '\n';
        text[len] = '\0';
        //调用writeDisk()函数将通过键盘键入的内容写到文件中
        temp = writeDisk(fd, text, len);
        if (temp != -1)
        {
            //实际写入总字节数
            all_len += temp;
        }
        if (temp < len)
        {
            cout << "Error: write error" << endl;
            break;
        }
    }
    //返回实际写入的字节数
    return all_len;
}
