#pragma comment(lib, "ws2_32.lib") //动态链接库

#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include<string.h>
#include<fstream>
#include"protocol_client.h"

int main(int argc, char** argv)
{ 
   

	HANDLE send_rec;
    if (argc <= 1)
    {
        printf("命令行没有ip \n");
        system("pause");
        return -1;
    }
	RecThread_para rec_para;
	strcpy(rec_para.ipaddress, argv[1]);
    char filename[20] = "client2server.txt";
    send_rec = CreateThread(NULL,0, (LPTHREAD_START_ROUTINE)Socket_rec,&rec_para,0,NULL);
    Socket_send(filename,argv[1]);
	WaitForSingleObject(send_rec,100000);

	printf("Finish!");
	return 1;
}
