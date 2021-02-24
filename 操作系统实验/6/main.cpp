#include <iostream>

#include "main.h"

using namespace std;

//命令错误
void UnNameError()
{
    cout << "Error: this command is not defined" << endl;
}

//查看帮助
void help()
{
    cout << "  new: 建立一个新的简单文件系统" << endl;
    cout << "  sfs: 打开一个简单文件系统" << endl;
    cout << "  exit: 退出打开的简单文件系统" << endl;
    cout << "  mkdir: 创建子目录" << endl;
    cout << "  rmdir: 删除子目录" << endl;
    cout << "  ls: 显示目录" << endl;
    cout << "  cd: 更改当前目录" << endl;
    cout << "  create: 创建文件" << endl;
    cout << "  open: 打开文件" << endl;
    cout << "  close: 关闭文件" << endl;
    cout << "  read: 读文件" << endl;
    cout << "  write: 写文件" << endl;
    cout << "  delete: 删除文件" << endl;
}

int main()
{
    cout << "input help to look help" << endl;
    //命令行输入
    string cmd;
    //命令和参数
    string request, parameter;
    int index; //分割cmd
    //标记是否打开文件系统
    bool openSystem = false;
    //打开文件系统的名称
    string openedSystem;
    while (true)
    {
        //输出路径
        if (openSystem)
        {
            cout << OPEN_FILE_LIST[CURRENT_DIR].dir << ">";
        }
        getline(cin, cmd);
        index = cmd.find(' ');
        parameter = cmd.substr(index + 1, cmd.length());
        request = cmd.substr(0, index);
        if (request == "help")
        {
            help();
        }
        else if (request == "new")
        {
            createFileSystem(parameter);
        }
        else if (request == "sfs")
        {
            if (openSystem)
            {
                cout << "file system " << openedSystem << " has opened" << endl;
            }
            else
            {
                openSystem = openFileSystem(parameter);
                if (openSystem)
                {
                    openedSystem = parameter;
                }
            }
        }
        else if (request == "exit")
        {
            if (openSystem)
            {
                exitSystem(openedSystem);
                break;
            }
            else
            {
                cout << "Error: please open a file system first" << endl;
            }
        }
        else if (request == "mkdir")
        {
            if (openSystem)
            {
                makeDirectory(parameter);
            }
            else
            {
                cout << "Error: please open a file system first" << endl;
            }
        }
        else if (request == "rmdir")
        {
            if (openSystem)
            {
                deleteDirectory(parameter);
            }
            else
            {
                cout << "Error: please open a file system first" << endl;
            }
        }
        else if (request == "ls")
        {
            if (openSystem)
            {
                fileList();
            }
            else
            {
                cout << "Error: please open a file system first" << endl;
            }
        }
        else if (request == "cd")
        {
            if (openSystem)
            {
                changeDirectory(parameter);
            }
            else
            {
                cout << "Error: please open a file system first" << endl;
            }
        }
        else if (request == "create")
        {
            if (openSystem)
            {
                createFile(parameter);
            }
            else
            {
                cout << "Error: please open a file system first" << endl;
            }
        }
        else if (request == "open")
        {
            if (openSystem)
            {
                CURRENT_DIR = open(parameter);
                if (CURRENT_DIR != -1)
                {
                    cout << "open " << parameter << " successfully" << endl;
                }
            }
            else
            {
                cout << "Error: please open a file system first" << endl;
            }
        }
        else if (request == "close")
        {
            char closeFileName[11];
            strcpy(closeFileName, OPEN_FILE_LIST[CURRENT_DIR].fcb.filename);
            if (openSystem)
            {
                if (OPEN_FILE_LIST[CURRENT_DIR].fcb.attribute == FILE_TYPE)
                {
                    CURRENT_DIR = close(CURRENT_DIR);
                    if (CURRENT_DIR != -1)
                    {
                        cout << "close " << closeFileName << " successfully" << endl;
                    }
                }
                else
                {
                    cout << "Error: there is not a file opened" << endl;
                }
            }
            else
            {
                cout << "Error: please open a file system first" << endl;
            }
        }
        else if (request == "read")
        {
            if (openSystem)
            {
                if (OPEN_FILE_LIST[CURRENT_DIR].fcb.attribute == FILE_TYPE)
                {
                    read(CURRENT_DIR, OPEN_FILE_LIST[CURRENT_DIR].fcb.length);
                }
                else
                {
                    cout << "Error: there is not a file opened" << endl;
                }
            }
            else
            {
                cout << "Error: please open a file system first" << endl;
            }
        }
        else if (request == "write")
        {
            if (openSystem)
            {
                if (OPEN_FILE_LIST[CURRENT_DIR].fcb.attribute == FILE_TYPE)
                {
                    write(CURRENT_DIR);
                }
                else
                {
                    cout << "Error: there is not a file opened" << endl;
                }
            }
            else
            {
                cout << "Error: please open a file system first" << endl;
            }
        }
        else if (request == "delete")
        {
            if (openSystem)
            {
                if (OPEN_FILE_LIST[CURRENT_DIR].fcb.attribute != FILE_TYPE)
                {
                    deleteFile(parameter);
                }
                else
                {
                    cout << "Error: there is not a file opened" << endl;
                }
            }
            else
            {
                cout << "Error: please open a file system first" << endl;
            }
        }
        else
        {
            UnNameError();
        }
    }
    return 0;
}
