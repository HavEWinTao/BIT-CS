#include <iostream>

#include "main.h"

using namespace std;

//�������
void UnNameError()
{
    cout << "Error: this command is not defined" << endl;
}

//�鿴����
void help()
{
    cout << "  new: ����һ���µļ��ļ�ϵͳ" << endl;
    cout << "  sfs: ��һ�����ļ�ϵͳ" << endl;
    cout << "  exit: �˳��򿪵ļ��ļ�ϵͳ" << endl;
    cout << "  mkdir: ������Ŀ¼" << endl;
    cout << "  rmdir: ɾ����Ŀ¼" << endl;
    cout << "  ls: ��ʾĿ¼" << endl;
    cout << "  cd: ���ĵ�ǰĿ¼" << endl;
    cout << "  create: �����ļ�" << endl;
    cout << "  open: ���ļ�" << endl;
    cout << "  close: �ر��ļ�" << endl;
    cout << "  read: ���ļ�" << endl;
    cout << "  write: д�ļ�" << endl;
    cout << "  delete: ɾ���ļ�" << endl;
}

int main()
{
    cout << "input help to look help" << endl;
    //����������
    string cmd;
    //����Ͳ���
    string request, parameter;
    int index; //�ָ�cmd
    //����Ƿ���ļ�ϵͳ
    bool openSystem = false;
    //���ļ�ϵͳ������
    string openedSystem;
    while (true)
    {
        //���·��
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
