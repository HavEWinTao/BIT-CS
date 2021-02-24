#include <iomanip>

#include "ls.h"
#include "utils.h"

void fileList()
{
    FCB *fcb;
    char text[MAX_TEXT];
    int rbn;
    OPEN_FILE_LIST[CURRENT_DIR].count = 0;
    //读出当前目录文件内容到内存
    rbn = readDisk(CURRENT_DIR, OPEN_FILE_LIST[CURRENT_DIR].fcb.length, text);
    fcb = (FCB *)text;
    //便利FCB表
    for (int i = 0; i < rbn / sizeof(FCB); i++)
    {
        //按照一定的格式输出
        if (fcb->free)
        {
            //文件夹
            if (fcb->attribute == FOLDER_TYPE)
            {
                int blank_len = 13 - strlen(fcb->filename);
                cout << fcb->filename;
                while (blank_len--)
                {
                    cout << " ";
                }
                cout << "<DIR>"
                     << "      ";
                cout << ((fcb->date >> 9) + 1980) << "/"
                     << setw(2) << setfill('0') << ((fcb->date >> 5) & 0x000f) << "/"
                     << setw(2) << setfill('0') << (fcb->date & 0x001f) << "      ";
                cout << setw(2) << setfill('0') << (fcb->time >> 11) << ":"
                     << setw(2) << setfill('0') << ((fcb->time >> 5) & 0x003f) << ":"
                     << setw(2) << setfill('0') << (fcb->time & 0x001f * 2) << endl;
            }
            else if (fcb->attribute == FILE_TYPE)
            { //文件
                int blank_len = 12 - strlen(fcb->filename);
                cout << fcb->filename;
                while (blank_len--)
                {
                    cout << " ";
                }
                cout << setw(4) << setfill(' ') << fcb->length << "B"
                     << "      ";
                cout << ((fcb->date >> 9) + 1900) << "/"
                     << setw(2) << setfill('0') << ((fcb->date >> 5) & 0x000f) << "/"
                     << setw(2) << setfill('0') << (fcb->date & 0x001f) << "      ";
                cout << setw(2) << setfill('0') << (fcb->time >> 11) << ":"
                     << setw(2) << setfill('0') << ((fcb->time >> 5) & 0x003f) << ":"
                     << setw(2) << setfill('0') << (fcb->time & 0x001f * 2) << endl;
            }
        }
        fcb++;
    }
}
