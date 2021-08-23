#pragma once
#pragma comment(lib, "ws2_32.lib") //动态链接库
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <WinSock2.h>
#include<Windows.h>
#include<time.h>

#define PF_num 6000
#define MAX_PKT 1024
#define UDPPort_send 10011
#define UDPPort_rec 10010
#define UDPPort_send_1 10012
#define UDPPort_rec_1 10013
#define DataSize 4096
#define ErrorRate 0
#define LostRate 0
#define SWSize 7
#define InitSeqNO 1
#define TimeOut 1000
#define SlideWindow 10

typedef unsigned int seq_nr;
typedef struct { unsigned char data[MAX_PKT]; }
packet;

typedef enum { data, ack, nak, info }
frame_kind;

typedef struct frame
{
	frame_kind kind;
	seq_nr seq;
	seq_nr ack;
	packet info;
	char filename[20];
	int frame_num;
	unsigned short CheckSum;
} frame;

typedef struct Side_ACK {
	int side_head;
	int side_tail;
	int nak_flag;
}Side_ACK;

typedef struct Socket_send_ACK_Para {
	int side_head;
	int side_tail;
}Socket_send_ACK_Para;

typedef struct Socket_rec_ACK_Para {
	char ipaddress[10];
	int frame_num_now;
	int nak_flag;
	int nak_seq;
	int frame_sum;
} Socket_rec_ACK_Para;

typedef struct RecThread_para {
	char ipaddress[10];
}RecThread_para;


unsigned short CRC16_CCITT(unsigned char* pdata, int len) {
	unsigned short crc = 0x0000;//初始值 
	int i, j;
	for (j = 0; j < len; j++)
	{
		crc ^= pdata[j];

		for (i = 0; i < 8; i++) {
			if ((crc & 0x0001) > 0) {
				crc = (crc >> 1) ^ 0x8408;//0x1021 翻转  0x8408
			}
			else
				crc >>= 1;
		}
	}
	return crc;
}


int Gen_Packet(char* filename, packet* Packet_set)
{
	FILE* myfile = NULL;
	char buf;
	myfile = fopen(filename, "r");
	int index = 0;
	int i = 0;
	while ((Packet_set[index].data[i] = fgetc(myfile)) != 0x000000FF)
	{
		i++;
		if (i == 1024)
		{
			i = 0;
			index++;
		}
	}
	//Packet_set[index].data[i] = EOF;0x000000FF
	fclose(myfile);
	return index + 1;
}

void Gen_Frame(packet* source, int packet_num, frame* frame_send)
{
	for (int i = 1; i <= packet_num; i++)
	{
		//packet temp;
		//temp = *source;
		frame_send[i].info = *source;
		frame_send[i].kind = data;
		frame_send[i].seq = i;
		frame_send[i].ack = i % 2;
		frame_send[i].CheckSum = CRC16_CCITT(frame_send[i].info.data,1024);
		source += 1;
	}

}

void Gen_File(frame* frame_rec, char* filename, int frame_num)
{
	int sum = frame_num;
	int index = 1;
	FILE* file;
	file = fopen(filename, "w");
	while (sum >= 1)
	{
		int sub_index = 0;
		while (sub_index != 1023)
		{
			fputc(frame_rec[index].info.data[sub_index], file);
			sub_index++;
			if (frame_rec[index].info.data[sub_index] == 0x000000FF)
				goto p;
		}
		index++;
		sum--;
	}
p:
	fclose(file);
}

void Gen_Frame_1(frame* frame_1, char* filename, int frame_num)
{
	frame_1[0].kind = info;
	frame_1[0].frame_num = frame_num;
	frame_1[0].seq = 0;
	strcpy(frame_1[0].filename, filename);
}

DWORD WINAPI Socket_send_ACK(LPVOID lpParam)
{
	frame ack_rec;
	Side_ACK* side_ack = (Side_ACK*)lpParam;
	WSADATA wsaData;
	SOCKET ReceivingSocket;
	SOCKADDR_IN ReceiverAddr;
	SOCKADDR_IN SenderAddr;
	int SenderAddrSize;
	SenderAddrSize = sizeof(SenderAddr);
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	ReceivingSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	ReceiverAddr.sin_family = AF_INET;
	ReceiverAddr.sin_port = htons(UDPPort_rec);
	ReceiverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	bind(ReceivingSocket, (SOCKADDR*)&ReceiverAddr, sizeof(ReceiverAddr));
	//printf("成功建立SEND子线程\n");
	while (1)
	{
		int num = 0;
		char buffer[2048] = "\0";
		if (recvfrom(ReceivingSocket, buffer, 2048, 0, (SOCKADDR*)&SenderAddr, &SenderAddrSize) != SOCKET_ERROR)
		{
			ack_rec = *(frame*)&buffer;
			if (ack_rec.kind == ack&&num< ack_rec.seq)
			{
				side_ack->side_head = ack_rec.seq;
				num= ack_rec.seq;
				//printf("成功收到确认帧%d\n", ack_rec.seq);
			}
			else if (ack_rec.kind == nak)
			{
				side_ack->side_head = ack_rec.seq;
				side_ack->nak_flag = 1;
				num = ack_rec.seq;
			}
		}
	}
	
}

void Socket_send(char* filename, char* ipaddress)
{
	FILE* client_log_send;
	client_log_send = fopen("client_log_send.txt", "w+");
	time_t now;
	struct tm* tm_now;
	packet pac[PF_num];
	frame frm[PF_num];
	int packet_num = 0;
	HANDLE thread_ACK_rec;
	Side_ACK side_ack_para;
	side_ack_para.side_head = -1;
	side_ack_para.side_tail = SlideWindow;
    WSADATA wsaData;
	SOCKET SendingSocket;
	SOCKADDR_IN ReceiverAddr;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	SendingSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	ReceiverAddr.sin_family = AF_INET;
	ReceiverAddr.sin_port = htons(UDPPort_send);
	ReceiverAddr.sin_addr.S_un.S_addr = inet_addr(ipaddress);
	packet_num = Gen_Packet(filename, pac);
	Gen_Frame_1(frm, filename, packet_num);
	Gen_Frame(pac, packet_num, frm);
	thread_ACK_rec = CreateThread(NULL,0,Socket_send_ACK,&side_ack_para,0,NULL);//接收子线程
	int pre_turn_head=0;
	while (1)
	{
		int flag = 1;
		for (int i = pre_turn_head; i < side_ack_para.side_head; i++)
		{
			time(&now);
			tm_now = localtime(&now);
			fprintf(client_log_send,"【%d-%d-%d %d:%d:%d】pdu-to-send=%d,status=New,ackedNo=%d\n", tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec, i, side_ack_para.side_head);
			//printf("【%d-%d-%d %d:%d:%d】pdu-to-send=%d,status=New,ackedNo=%d\n", tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec, i, side_ack_para.side_head);
		}

	    if (side_ack_para.nak_flag == 1)
		{
			time(&now);
			tm_now = localtime(&now);
			//printf("校验错误重传%d\n", side_ack_para.side_head);
			fprintf(client_log_send,"【%d-%d-%d %d:%d:%d】pdu-to-send=%d,status=RT,ackedNo=%d\n", tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec, side_ack_para.side_head, side_ack_para.side_head);
			//printf("【%d-%d-%d %d:%d:%d】pdu-to-send=%d,status=RT,ackedNo=%d\n", tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec, side_ack_para.side_head, side_ack_para.side_head);
			side_ack_para.nak_flag = 0;
		}
		if(pre_turn_head==side_ack_para.side_head && pre_turn_head!=0)
		{
			time(&now);
			tm_now = localtime(&now);
			fprintf(client_log_send,"【%d-%d-%d %d:%d:%d】pdu-to-send=%d,status=TO,ackedNo=%d\n", tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec, side_ack_para.side_head, side_ack_para.side_head);
			//printf( "【%d-%d-%d %d:%d:%d】pdu-to-send=%d,status=TO,ackedNo=%d\n", tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec, side_ack_para.side_head, side_ack_para.side_head);
			//printf("----------------超时重传%d\n", side_ack_para.side_head);
		}
		while (flag < SlideWindow)
		{
			
			if (side_ack_para.side_head + flag > packet_num)
					break;
			char* buffer = (char*)&frm[side_ack_para.side_head+flag];
			if (sendto(SendingSocket, buffer, 2048, 0, (SOCKADDR*)&ReceiverAddr, sizeof(ReceiverAddr)) != SOCKET_ERROR)
			{
				//printf("【%d-%d-%d %d:%d:%d】pdu-to-send=%d,status=New,ackedNo=%d\n", tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec, side_ack_para.side_head + flag, side_ack_para.side_head);
				flag++;
			}
			else
			{
				printf("发送失败！\n");
			}
			
		}

        pre_turn_head = side_ack_para.side_head;
		Sleep(TimeOut);
		
		if (side_ack_para.side_head == packet_num)
		{
			TerminateThread(Socket_send_ACK, 0);
			printf("client-close-send_ack");
			break;
		}

	}
	fclose(client_log_send);
	closesocket(SendingSocket);
	WSACleanup();
	return ;
}


DWORD WINAPI Socket_rec_ACK(LPVOID lpParam)
{

	frame rec_ack;
	int frame_num_now = 0;
	Socket_rec_ACK_Para* socket_rec_ack_para = (Socket_rec_ACK_Para*)lpParam;
	WSADATA wsaData;
	SOCKET SendingSocket;
	SOCKADDR_IN ReceiverAddr;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	SendingSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	ReceiverAddr.sin_family = AF_INET;
	ReceiverAddr.sin_port = htons(UDPPort_send_1);
	ReceiverAddr.sin_addr.S_un.S_addr = inet_addr(socket_rec_ack_para->ipaddress);
	printf("%s\n", socket_rec_ack_para->ipaddress);
	int temp_flag = 0;
	while (1)
	{
		if (socket_rec_ack_para->frame_num_now > frame_num_now)
		{
			frame_num_now = socket_rec_ack_para->frame_num_now;
			rec_ack.kind = ack;
			rec_ack.seq = frame_num_now;
			char* buffer = (char*)&rec_ack;
			sendto(SendingSocket, buffer, 2048, 0, (SOCKADDR*)&ReceiverAddr, sizeof(ReceiverAddr));
			//printf("成功发送确认帧%d\n", frame_num_now);
			//printf("%d\n",rec_ack.seq);

			
		}
		else if (socket_rec_ack_para->nak_flag == 1)
		{
			rec_ack.kind = nak;
			rec_ack.seq = rec_ack.kind = ack;
			rec_ack.seq = socket_rec_ack_para->nak_seq;
			char* buffer = (char*)&rec_ack;
			sendto(SendingSocket, buffer, 2048, 0, (SOCKADDR*)&ReceiverAddr, sizeof(ReceiverAddr));
			socket_rec_ack_para->nak_flag = 0;
		}
	}
	return  0;


}


void Socket_rec(LPVOID lpParam)
{
	FILE* client_log_rec;
	client_log_rec = fopen("client_log_rec.txt", "w+");
	RecThread_para* recthread_para = (RecThread_para*)lpParam;
	char ipaddress[10];
	strcpy(ipaddress, recthread_para->ipaddress);
	time_t now;
	struct tm* tm_now;
	frame rfrm[PF_num];
	HANDLE thread_ACK_send;
	Socket_rec_ACK_Para socket_rec_ack_para;
	strcpy(socket_rec_ack_para.ipaddress, ipaddress);
	socket_rec_ack_para.frame_num_now = -1;
	socket_rec_ack_para.frame_sum = 0;
	socket_rec_ack_para.nak_flag = 0;
	socket_rec_ack_para.nak_seq = 0;
	WSADATA wsaData;
	SOCKET ReceivingSocket;
	SOCKADDR_IN ReceiverAddr;
	SOCKADDR_IN SenderAddr;
	int SenderAddrSize;
	SenderAddrSize = sizeof(SenderAddr);
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	ReceivingSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	ReceiverAddr.sin_family = AF_INET;
	ReceiverAddr.sin_port = htons(UDPPort_rec_1);
	ReceiverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	bind(ReceivingSocket, (SOCKADDR*)&ReceiverAddr, sizeof(ReceiverAddr));
	thread_ACK_send = CreateThread(NULL, 0, Socket_rec_ACK, &socket_rec_ack_para, 0, NULL);//发送子线程

	int index = 0;
	int frame_num = 0;
	char filename[20] = "\0";
	int Ret = 0;
	int frame_num_wanted = 0;
	frame temp;
	while (1)
	{
		char buffer[2048] = "\0";
		if ((Ret = recvfrom(ReceivingSocket, buffer, 2048, 0, (SOCKADDR*)&SenderAddr, &SenderAddrSize)) != SOCKET_ERROR)
		{
			temp = *(frame*)&buffer;
			if (temp.seq == frame_num_wanted)
			{
				if (CRC16_CCITT(temp.info.data, 1024) == temp.CheckSum || index == 0)//如果校验正确，则返回确认帧
				{
					time(&now);
					tm_now = localtime(&now);
					socket_rec_ack_para.frame_num_now = temp.seq;
					rfrm[index] = temp;
					fprintf(client_log_rec,"【%d-%d-%d %d:%d:%d】pdu_exp=%d,pdu_recv=%d,status=OK\n", tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec, frame_num_wanted, temp.seq);
					//printf("【%d-%d-%d %d:%d:%d】pdu_exp=%d,pdu_recv=%d,status=OK\n", tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec, frame_num_wanted, temp.seq);
					frame_num_wanted++;
					index++;
				}
				else
				{
					time(&now);
					tm_now = localtime(&now);
					socket_rec_ack_para.nak_flag = 1;
					socket_rec_ack_para.nak_seq = temp.seq;
					fprintf(client_log_rec,"【%d-%d-%d %d:%d:%d】pdu_exp=%d,pdu_recv=%d,status=DataErr\n", tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec, frame_num_wanted, temp.seq);
					//printf( "【%d-%d-%d %d:%d:%d】pdu_exp=%d,pdu_recv=%d,status=DataErr\n", tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec, frame_num_wanted, temp.seq);
					//printf("帧%d校验错误\n", temp.seq);
				}
			}
			else
			{
				time(&now);
				tm_now = localtime(&now);
				fprintf(client_log_rec,"【%d-%d-%d %d:%d:%d】pdu_exp=%d,pdu_recv=%d,status=NoErr\n", tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec, frame_num_wanted, temp.seq);
				//printf("【%d-%d-%d %d:%d:%d】pdu_exp=%d,pdu_recv=%d,status=NoErr\n", tm_now->tm_year + 1900, tm_now->tm_mon + 1, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec, frame_num_wanted, temp.seq);

			}


			if (temp.seq == 0)
			{
				frame_num = temp.frame_num;
				strcpy(filename, temp.filename);
				socket_rec_ack_para.frame_sum = frame_num;
			}

		}
		if (rfrm[index - 1].seq == frame_num)
		{
			rfrm[index - 1].info.data[strlen((const char*)rfrm[index - 1].info.data)] = '\0';
			printf("readytoclose\n");
			TerminateThread(thread_ACK_send,1);
			break;
		}
	}
	fclose(client_log_rec);
	Gen_File(rfrm, filename, frame_num);
	closesocket(ReceivingSocket);
	WSACleanup();
	return ;
}








