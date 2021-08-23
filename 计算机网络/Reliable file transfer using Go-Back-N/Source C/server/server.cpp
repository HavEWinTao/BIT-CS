#pragma comment(lib, "ws2_32.lib") //动态链接库

#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include<string.h>
#include"protocol_server.h"

int main(int argc, char** argv)
{
	HANDLE rec_send;
    if (argc <= 1)
    {
        printf("命令行没有ip \n");
        system("pause");
        return -1;
    }
	SendThread_para send_para;
	strcpy(send_para.filename, "server2client.txt");
	strcpy(send_para.ipaddress,argv[1]);
    rec_send = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Socket_send, &send_para, 0, NULL);
    Socket_rec(argv[1]);
	WaitForSingleObject(rec_send, 100000);
	system("pause");
    return 1;
}
