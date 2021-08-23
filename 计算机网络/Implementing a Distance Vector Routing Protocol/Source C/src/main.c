#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include "config.h"

/******* �������� *******/
void initSys(char* node_id);																										// �ڵ�ϵͳ���ó�ʼ������������Ϊϵͳ�����ļ�ָ��
void initNode(char* node_init_name, char* this_id, char* this_port);					// �ڵ��ʼ������������Ϊ��ʼ���ļ�ָ��
void initTimeSem(timemutex* tmutex, char* name);														// ��ʼ��ʱ���ź���
void initSem(HANDLE* mutex, char* name, int init_value, int max_value);					//	��ʼ����ͨ�ź���

DWORD WINAPI sendTimerFunc(LPVOID lpParam);													// ���Ͷ�ʱ���߳�
DWORD WINAPI recTimerFunc(LPVOID lpParam);													// ���ն�ʱ���߳�
DWORD WINAPI recThreadFunc(LPVOID lpParam);											// �������߳�ִ�к��������ڽ����ڽӽڵ�Ľ�����Ϣ
DWORD WINAPI sendThreadFunc(LPVOID lpParam);											// �������߳�ִ�к���

void sendPart();
void receivePart();
void closeAll();
void parseAll();
void printInitNode();
void printNodeList();
int dv(noderec* node_for_rec, int neibor_num);			// ����·�ɱ����

/********* ȫ�ֱ������� *********/
const char sys_init_path[100] = "E:\\Study\\3rd_University_Down\\ComputerNetwork\\Experiment\\ʵ���� С����ҵ2\\Code\\sys.txt";
const char node_init_path_src[100] = "E:\\Study\\3rd_University_Down\\ComputerNetwork\\Experiment\\ʵ���� С����ҵ2\\Code\\node_init\\";
const char log_save_path_src[100] = "E:\\Study\\3rd_University_Down\\ComputerNetwork\\Experiment\\ʵ���� С����ҵ2\\Code\\log\\";
char log_save_path[120];
syssetting sys_set;					// ϵͳ��������
HANDLE mutex;					// ·�ɱ��д������
HANDLE st_h;
timemutex stm;					
HANDLE rt_h;
HANDLE send_h;
HANDLE rec_h;
HANDLE nt_h;
timemutex ntm;
HANDLE rec_mutex;		// ���ձ�־�ź���
static int time_lim[100] = { 0 };		// ���ձ�־��0Ϊ��ǰ����δ���գ�1Ϊ��ǰ�����ѽ���
static int rec_flag[100] = { 0 };		// ���սڵ㽡����־��0Ϊ������1Ϊ崻�
HANDLE nrec_h;
static node node_list[100][100];				// ·�ɱ�Ĭ���������˵���Ϊ100����Ϊ�����ڽӽڵ㣬��Ϊ���нڵ�
static int flag = 1;		// �����߳��˳���־��1���У�0�˳�����ʼΪ1
static int seq = 0;		// sequence


int main(int argc, char* argv[])
{
	// �������������ֹ������
	if (argc != 4)
	{
		printf("Command values error: please call the command in the format of \"Programe_Name Node_id Node_UDP_Port Initialization_File_Name \".\n");
		exit(-1);
	}
	initSys(argv[1]);													//��ʼ���ڵ�ϵͳ����
	initNode(argv[3], argv[1], argv[2]);			// ��ʼ���ڵ㣬�����ڽӽڵ����ʼ·�ɱ�
	initSem(&mutex, "MUTEX", 1, 1);
	
	sendPart();
	receivePart();

	char key_cmd;
	while (1)
	{
		printf("Input cmd.\nk --- kill this node\np --- parse this node\ns --- restart this node\n");
		scanf("%c", &key_cmd);
		getc(stdin);
		key_cmd = key_cmd > 'Z' ? key_cmd - 32 : key_cmd;
		if (key_cmd == 'K')		// ����Ȼ����ɱ
		{
			flag = 0;
			break;
		}
		else if (key_cmd == 'P')
		{
			flag = 0;
		}
		else if (key_cmd == 'S')
		{
			flag = 1;
			sendPart();
			receivePart();
		}
	}

	closeAll();
	return 0;
}

DWORD WINAPI sendTimerFunc(LPVOID lpParam)
{
	timemutex tm = *(timemutex*)lpParam;
	time_t start, current = 0;
	start = clock();
	while (flag)
	{
		current = clock();
		if ((current - start) >= tm.delay)
		{
			start = clock();
			ReleaseSemaphore(tm.time_h, 1, NULL);	// ����
		}
	}
	return 0;
}

DWORD WINAPI recTimerFunc(LPVOID lpParam)
{
	// �ָ�ʱ�����������״��
	memset(time_lim, 0, sizeof(time_lim));
	memset(rec_flag, 0, sizeof(rec_flag));
	timemutex tm = *(timemutex*)lpParam;
	time_t start[100];	// �洢�����ڽӽڵ����һ�ν���ʱ��
	time_t current = 0;
	for (int i = 1; i <= sys_set.neibor_num; i++)
	{
		start[i] = clock();
	}
	printf("** REC Timer: start init finished.\n");
	while (flag)
	{
		current = clock();
		// ÿ��ѭ��������нڵ�Ľ���״��
		for (int i = 1; i <= sys_set.neibor_num; i++)
		{
			if (time_lim[i] == 0)	// ���Ϊi���ڽӽڵ㵱ǰδ����
			{
				if (current - start[i] >= sys_set.max_delay && rec_flag[i] != 1)	// �ѳ�ʱ��δ�ò�����
				{
					WaitForSingleObject(rec_mutex, INFINITE);
					rec_flag[i] = 1;	// ��ָ��id�ڵ���Ϊ������
					ReleaseSemaphore(rec_mutex, 1, NULL);
					// printf("REC Timer: �ѽ�%d�ڵ���Ϊ����������־Ϊ %d\n", i, rec_flag[i]);
				}
			}
			else	// �ѽ��գ����ñ�־��i�ڵ��Ӧ����һ��ʱ��
			{
				WaitForSingleObject(rec_mutex, INFINITE);
				time_lim[i] = 0;		// ���ý��ձ�־
				ReleaseSemaphore(rec_mutex, 1, NULL);
				start[i] = clock();
			}
		}
	}
	return 0;
}

DWORD WINAPI recThreadFunc(LPVOID lpParam)
{
	int node_i = 0;	// ��ʱ���յ��ڽӽڵ��ڱ���·�ɱ�����λ��
	int rec_seq = 0;	// ����֡���

	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 2);
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		printf("Error: WSAStartup failed.\n");
		exit(-1);
	}
	SOCKET ReceivingSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (ReceivingSocket == INVALID_SOCKET)
	{
		printf("Error: serSocket create failed.\n");
		exit(-1);
	}
	SOCKADDR_IN ReceiverAddr;
	ReceiverAddr.sin_family = AF_INET;
	ReceiverAddr.sin_port = htons(node_list[0][0].port);
	ReceiverAddr.sin_addr.S_un.S_addr = inet_addr(node_list[0][0].ip);
	
	if (bind(ReceivingSocket, (SOCKADDR*)&ReceiverAddr, sizeof(ReceiverAddr)) == SOCKET_ERROR)
	{
		printf("Error: node_i %d bind failed.\n", node_i);
		closesocket(ReceivingSocket);
		exit(-1);
	}
	SOCKADDR_IN remoteAddr;
	int nAddrLen = sizeof(remoteAddr);

	while (flag)
	{
		// ѭ����������ڽӽڵ�Ľ���״��
		WaitForSingleObject(rec_mutex, INFINITE);
		for (int i = 1; i <= sys_set.neibor_num; i++)
		{
			// �ڴ˴������²������ڵ㣬�����ڵ㽫�����нڵ���ɼ����Ϊ������̬�������˿�
			if (rec_flag[i] == 1 && node_list[0][i].distance < INFINITE)	// �ýڵ㲻������δ����Ϊ���ɴ�
			{
				printf("node %d, id=%c, unhealthy.\n", i, node_list[0][i].id);
				WaitForSingleObject(mutex, INFINITE);	// ·�ɱ���
				node_list[0][i].distance = INFINITE;
				//// �ֶ����¸��ڽӽڵ��cost״��
				//for (int j = 0; j < sys_set.node_num; j++)
				//{
				//	node_list[i][j].distance = INFINITE;		// �ýڵ�ĺ���ȫ����INF
				//}
				//// ֮����±���·�ɱ������в����˸ýڵ���Ϊ��һ���Ľڵ���Ϣ����Ҫ�����Ӧ���ҵ���С���滻
				//for (int m = 1; m < sys_set.node_num; m++)
				//{
				//	if (node_list[0][m].neighbor == node_list[0][i].id)	// �ҵ���һ���Ǹ�ʧЧ�ڵ�Ľڵ���
				//	{
				//		node_list[0][m].distance = INFINITE;	// ���Ƚ��ýڵ���INF
				//		for (int k = 1; k <= sys_set.neibor_num; k++)	// Ȼ��������������ڽӽڵ㵽�ýڵ��cost�Ƿ��и��ţ�ѭ����Ѱ������
				//		{
				//			// ��һ�����Ǹýڵ�
				//			if (k != i)
				//			{
				//				// �����С�ڵ�ǰ��INF��
				//				if (node_list[k][m].distance + node_list[0][k].distance < node_list[0][m].distance)
				//				{
				//					// С�������distance��neibor��Ϣ
				//					node_list[0][m].distance = node_list[k][m].distance + node_list[0][k].distance;
				//					node_list[0][m].neighbor = node_list[0][k].id;
				//				}
				//			}
				//		}
				//	}
				//}
				int change = 1;
				// �˴�Ҳ���Դ��������̣߳��ݲ���
				ReleaseSemaphore(mutex, 1, NULL);	// �ͷ�·�ɱ���
			}
		}
		ReleaseSemaphore(rec_mutex, 1, NULL);

		// �����ϣ���ʼ��������Ϣ
		char* buffer = (char*)malloc(sizeof(noderec));
		noderec* node_for_rec = (noderec*)malloc(sizeof(noderec));
		int recv_len = recvfrom(ReceivingSocket, buffer, sizeof(noderec), 0, (SOCKADDR*)&remoteAddr, &nAddrLen);
		if (recv_len != SOCKET_ERROR)
		{
			memcpy(node_for_rec, buffer, sizeof(noderec));
			printf("******* node_for_recv *******\n");
			printf("recv_len: %d\n", recv_len);
			for (int m = 0; m < sys_set.node_num; m++)
			{
				printf("id: %c, distance: %f\n", node_for_rec->list[m].id, node_for_rec->list[m].distance);
			}
			printf("******* finished *******\n");
			WaitForSingleObject(mutex, INFINITE);	// ���յ������˲ſ�ʼ����·�ɱ���
			for (int m = 1; m <= sys_set.neibor_num; m++)		// �ұ��ν��յ����ڽӽڵ�Ķ�Ӧλ��
			{
				if (node_list[0][m].id == node_for_rec->rec_id)	
				{
					node_i = m;
					rec_seq = node_for_rec->rec_seq;
					printf("rec_seq: %d\n",rec_seq);
					WaitForSingleObject(rec_mutex, INFINITE);
					time_lim[m] = 1;	// �Խ��ձ�־����
					rec_flag[m] = 0;	// ����ýڵ�պ���ʧЧ�ڵ㣬��˵���ѻָ�����������Ϊ0-����
					ReleaseSemaphore(rec_mutex, 1, NULL);
				}
			}
			int change = dv(node_for_rec, node_i);		// ·�ɱ��Ƿ��и���
			// �˴����Ը���change����sendmutex�����������̣߳��ݲ���
			
#ifdef LOG
			FILE* log_save_p = fopen(sys_set.log_save_path, "a");		// ��log�ļ�
			if (log_save_p == NULL)
			{
				printf("Error: log_save_p open failed.\n");
				exit(-1);
			}
			char log_str[1000];
			char int_str[35];
			memset(log_str, 0, sizeof(log_str));
			memset(int_str, 0, sizeof(int_str));

			time_t save_time;		// ��ȡ��ǰʱ����ַ���
			struct tm* now;
			time(&save_time);
			now = gmtime(&save_time);
			char* date = asctime(now);

			strcpy(log_str, date);
			strcat(log_str, "## Received. Source Node = ");
			log_str[strlen(log_str)] = node_for_rec->rec_id;
			// strcat(log_str, node_list[0][node_i].id);
			strcat(log_str, "; Sequence Number = ");
			strcat(log_str, itoa(rec_seq, int_str, 10));
			strcat(log_str, "\n");
			for (int i = 0; i < sys_set.node_num; i++)
			{
				strcat(log_str, "DestNode = ");
				char tmpid = node_for_rec->list[i].id;
				log_str[strlen(log_str)] = tmpid;
				//strcat(log_str, &(node_for_rec->list[i].id));
				strcat(log_str, "; Distance = ");
				char float_str[35];
				memset(float_str, 0, sizeof(float_str));
				sprintf(float_str, "%f", node_for_rec->list[i].distance);
				strcat(log_str, float_str);
				strcat(log_str, "; Neighbor = ");
				strcat(log_str, &node_list[0][node_i].id);	// ���յ�������neibor���Ǹ��ڽӽڵ�
				strcat(log_str, "\n");
			}
			strcat(log_str, "\n");
			printf("%s", log_str);
			fputs(log_str, log_save_p);	// ��logд��log�ļ�
			fclose(log_save_p);
#endif
			ReleaseSemaphore(mutex, 1, NULL);	// �ͷ�·�ɱ���
		}
		else
		{
			printf("Error: recv_len is %d.\n", recv_len);
		}

		free(buffer);
		free(node_for_rec);
	}
	closesocket(ReceivingSocket);
	WSACleanup();
	printf("RecThread is finished.\n");
	return 0;
}

DWORD WINAPI sendThreadFunc(LPVOID lpParam)
{
	while (flag)
	{
		WaitForSingleObject(stm.time_h, INFINITE);	// ����ʱ����
		WaitForSingleObject(mutex, INFINITE);	// ���·�ɱ���

		FILE* log_save_p = fopen(sys_set.log_save_path, "a");		// ��log�ļ�
		if (log_save_p == NULL)
		{
			printf("Error: log_save_p open failed.\n");
			exit(-1);
		}

		noderec *node_for_send = (noderec*)malloc(sizeof(noderec));			// ������·�ɱ���
		node_for_send->rec_id = node_list[0][0].id;
		node_for_send->rec_seq = seq;
		for (int i = 0; i < sys_set.node_num; i++)	// ���·�ɱ��������нڵ���Ϣ
		{
			node_for_send->list[i].id = node_list[0][i].id;
			node_for_send->list[i].distance = node_list[0][i].distance;
		}

		for (int i = 1; i <= sys_set.neibor_num; i++)	// ���ڽӽڵ㷢��·�ɱ�ֻ���ڽӽڵ�
		{
			if (node_list[0][i].distance >= sys_set.max_distance) continue;	// �������ɴ�ڵ㣬�ýڵ㲻������Ϣ
			WSADATA wsaData;
			SOCKET SendingSocket;
			SOCKADDR_IN ReceiverAddr;
			WSAStartup(MAKEWORD(2, 2), &wsaData);
			SendingSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			ReceiverAddr.sin_family = AF_INET;
			ReceiverAddr.sin_port = htons(node_list[0][i].port);
			ReceiverAddr.sin_addr.S_un.S_addr = inet_addr(node_list[0][i].ip);
			char* buffer = (char*)malloc(sizeof(noderec));
			memcpy(buffer, node_for_send, sizeof(noderec));
			int send_len = sendto(SendingSocket, buffer, sizeof(noderec), 0, (SOCKADDR*)&ReceiverAddr, sizeof(ReceiverAddr));
			closesocket(SendingSocket);
			WSACleanup();

			printf("******* node_for_send *******\n");
			printf("send_len: %d\n", send_len);
			for (int m = 0; m < sys_set.node_num; m++)
			{
				printf("id: %c, distance: %f\n", node_for_send->list[m].id, node_for_send->list[m].distance);
			}
			printf("******* finished *******\n\n");
			
#ifdef LOG
			char log_str[1000];
			char int_str[35];
			memset(log_str, 0, sizeof(log_str));
			memset(int_str, 0, sizeof(int_str));

			time_t save_time;		// ��ȡ��ǰʱ����ַ���
			struct tm* now;
			time(&save_time);
			now = gmtime(&save_time);
			char* date = asctime(now);

			strcpy(log_str, date);
			strcat(log_str, "## Sent. Source Node = ");
			strcat(log_str, &node_list[0][0].id);
			strcat(log_str, "; Sequence Number = ");
			strcat(log_str, itoa(node_for_send->rec_seq, int_str, 10));
			strcat(log_str, "\n");
			for (int i = 0; i < sys_set.node_num; i++)
			{
				strcat(log_str, "DestNode = ");
				strcat(log_str, &node_list[0][i].id);
				strcat(log_str, "; Distance = ");
				char float_str[35];
				memset(float_str, 0, sizeof(float_str));
				sprintf(float_str, "%f", node_list[0][i].distance);
				strcat(log_str, float_str);
				strcat(log_str, "; Neighbor = ");
				strcat(log_str, &node_list[0][i].neighbor);
				strcat(log_str, "\n");
			}
			strcat(log_str, "\n");
			printf("%s", log_str);
			fputs(log_str, log_save_p);	// ��logд��log�ļ�
#endif
			free(buffer);
			seq++;
		}
		free(node_for_send);
		fclose(log_save_p);	// �ر�log�ļ�
		ReleaseSemaphore(mutex, 1, NULL);	// �ͷ�·�ɱ���
	}
	return 0;
}

void sendPart()
{
	// ������ʼ��ʱ���߳�
	stm.time_h = 0;
	stm.delay = sys_set.sys_delay;
	if (&stm == NULL)
	{
		printf("&stm is NULL\n");
	}
	initTimeSem(&stm, "SEND");
	
	DWORD st_h_id;
	st_h = CreateThread(NULL, 0, sendTimerFunc, &stm, 0, &st_h_id);
	if (st_h == NULL)
	{
		printf("Error: st_h create failed.\n");
		exit(-1);
	}
	
	// ���������߳�
	DWORD send_h_id;
	send_h = CreateThread(NULL, 0, sendThreadFunc, NULL, 0, &send_h_id);
	if (send_h == NULL)
	{
		printf("Error: send_h create failed.\n");
		exit(-1);
	}
}

void receivePart()
{
	// ��ʼ�������ź���
	initSem(&rec_mutex, "RECMUTEX", 1, 1);
	// ������ʼ��ʱ���߳�
	ntm.time_h = 0;
	ntm.delay = sys_set.sys_delay;
	if (&ntm == NULL)
	{
		printf("&ntm is NULL\n");
	}
	initTimeSem(&ntm, "REC");

	DWORD nt_id;
	nt_h = CreateThread(NULL, 0, recTimerFunc, &ntm, 0, &nt_id);
	if (nt_h == NULL)
	{
		printf("Error: nt_h create failed.\n");
		exit(-1);
	}

	// ���������߳�
	DWORD nrec_h_id;
	nrec_h = CreateThread(NULL, 0, recThreadFunc, NULL, 0, &nrec_h_id);	// ����ָ���ڵ���Ϣ
	if (nrec_h == NULL)
	{
		printf("Error: nrec_h create failed.\n");
		exit(-1);
	}
}

void closeAll()
{
	//WaitForSingleObject(st_h, INFINITE);	//	�ȴ��߳̽���
	CloseHandle(st_h);
	//WaitForSingleObject(send_h, INFINITE);
	CloseHandle(send_h);
	//WaitForSingleObject(rec_h, INFINITE);
	CloseHandle(rec_h);
	CloseHandle(mutex);
	CloseHandle(stm.time_h);
	//WaitForSingleObject(nt_h[i], INFINITE);
	CloseHandle(nt_h);
	CloseHandle(ntm.time_h);
	CloseHandle(nrec_h);
}

void parseAll()
{
	WaitForSingleObject(st_h, INFINITE);	//	ǿ���˳���ʱ��
	WaitForSingleObject(send_h, INFINITE);
	WaitForSingleObject(rec_h, INFINITE);
	WaitForSingleObject(nt_h, INFINITE);
}

int dv(noderec *node_for_rec, int neibor_num)
{
	int change = 0;	// ·�ɱ��Ƿ���±�־

	printf("DVǰ��\n");
	printNodeList();

	// ���Ƚ��յ�����Ϣ���ϵ�����·�ɾ�����
	for (int i = 0; i < sys_set.node_num; i++)
	{
		// �ڱ���·�ɱ�����ʶ��Ĳ��ֽ��в�ѯ������find_flag��־��ʾ�Ƿ��ҵ�
		if (node_for_rec->list[i].id == '#') continue;		// ·�������и�����ڣ�������
		int find_flag = 0;
		for (int k = 0; k < sys_set.ident_num; k++)
		{
			if (node_list[0][k].id == node_for_rec->list[i].id)	// �ҵ���
			{
				find_flag = 1;
				node_list[neibor_num][k].id = node_for_rec->list[i].id;
				node_list[neibor_num][k].distance = node_for_rec->list[i].distance;
			}
		}
		if (find_flag == 0)	// û�ҵ�
		{
			// �����½ڵ���Ϣ�������и�ֵ
			node_list[0][sys_set.ident_num].id = node_for_rec->list[i].id;
			node_list[0][sys_set.ident_num].distance = node_for_rec->list[i].distance + node_list[neibor_num][0].distance;
			node_list[0][sys_set.ident_num].neighbor = node_for_rec->list[i].id;
			node_list[neibor_num][sys_set.ident_num].id = node_for_rec->list[i].id;
			node_list[neibor_num][sys_set.ident_num].distance = node_for_rec->list[i].distance;
			sys_set.ident_num++;
		}
	}
	printf("DV��ǰ\n");
	printNodeList();

	//// �½ڵ���Ϣ�Ѿ�ȫ�����ݣ����±���·�ɱ�ע��˴��ض�����Ϊ�ڽӽڵ�֮һ
	//// ��������һ��Ϊ���ν��յ��Ľڵ�Ľڵ���и���
	//for (int i = 1; i < sys_set.node_num; i++)
	//{
	//	if (node_list[0][i].neighbor == node_list[0][neibor_num].id)
	//	{
	//		node_list[0][i].distance = node_list[0][neibor_num].distance + node_list[neibor_num][i].distance;
	//	}
	//}

	// �½ڵ���Ϣ�Ѿ�ȫ�����ݣ����±���·�ɱ�ע��˴��ض�����Ϊ�ڽӽڵ�֮һ
	// �������нڵ�
	// ��ǰ���¸ý��սڵ�
	node_list[0][neibor_num].distance = node_list[neibor_num][0].distance;
	for (int i = 1; i <= sys_set.node_num; i++)
	{
		if (node_list[neibor_num][i].id == '#') continue;	// ·�������иýڵ�����Ϣ��ֱ������
		if (node_list[0][i].neighbor == node_list[0][neibor_num].id)		// ��һ��Ϊ��ǰ���յĽڵ�
		{
			node_list[0][i].distance = node_list[0][neibor_num].distance + node_list[neibor_num][i].distance;
		}
		else
		{
			if ((node_list[0][neibor_num].distance + node_list[neibor_num][i].distance) < (node_list[0][i].distance))
			{
				node_list[0][i].distance = node_list[0][neibor_num].distance + node_list[neibor_num][i].distance;
				node_list[0][i].neighbor = node_list[0][neibor_num].id;
			}
		}
	}
	float tmp = INFINITE;
	tmp += 20;
	printf("INF+20: %f\n", tmp);
	printf("DV�к�\n");
	printNodeList();


	// �淶��
	for (int i = 0; i < sys_set.node_num; i++)
	{
		if (node_list[0][i].distance > sys_set.max_distance)
		{
			node_list[0][i].distance = INFINITE;
		}
	}
	printf("DV��\n");
	printNodeList();


	//for (int i = 1; i < sys_set.node_num; i++)
	//{
	//	// ���ٸ��Ǹ��±��ڽӽڵ������
	//	if (i != neibor_num)
	//	{
	//		for (int k = 1; k <= sys_set.neibor_num; k++)
	//		{
	//			if (node_list[k][i].distance + node_list[0][k].distance < node_list[0][i].distance && (node_list[k][i].distance + node_list[0][k].distance) <= sys_set.max_distance)
	//			{
	//				change = 1;
	//				node_list[0][i].distance = node_list[k][i].distance + node_list[0][k].distance;
	//				node_list[0][i].neighbor = node_list[0][k].id;
	//			}
	//		}
	//	}
	//}

	return change;
}

void initSys(char* node_id)
{
	printf("initsys node_id: %s\n", node_id);
	FILE* sys_conf_p = fopen(sys_init_path, "r");		// ��ȡϵͳ�����ļ�
	if (sys_conf_p == NULL)
	{
		printf("Error: sys_conf_p open failed.\n");
		exit(-1);
	}

	// ���ж�ȡ��strtok��":"�ָ��Ӧʶ�𲢸�ֵ
	char tmp[100];
	memset(tmp, 0, sizeof(tmp));
	while (fgets(tmp, 100, sys_conf_p) != NULL)
	{
		char* str_p;
		str_p = strtok(tmp, ":");
		if (strcmp("Frequency", str_p) == 0)
		{
			str_p = strtok(NULL, ":");
			sys_set.sys_delay = atoi(str_p);
		}
		else if (strcmp("Unreachable", str_p) == 0)
		{
			str_p = strtok(NULL, ":");
			sys_set.max_distance = atof(str_p);
		}
		else if (strcmp("MaxValidTime", str_p) == 0)
		{
			str_p = strtok(NULL, ":");
			sys_set.max_delay = atoi(str_p);
		}
		else if (strcmp("Node_number", str_p) == 0)
		{
			str_p = strtok(NULL, ":");
			sys_set.node_num = atoi(str_p);
		}
		memset(tmp, 0, sizeof(tmp));
	}
	// ƴ��log�洢·��
	strcpy(sys_set.log_save_path, log_save_path_src);
	strcat(sys_set.log_save_path, node_id);
	strcat(sys_set.log_save_path, "_log.txt");

	printf("sys_delay: %d\n", sys_set.sys_delay);
	printf("max_distance: %f\n", sys_set.max_distance);
	printf("max_delay: %d\n", sys_set.max_delay);
	printf("node_num: %d\n", sys_set.node_num);
	printf("log_save_path: %s\n", sys_set.log_save_path);
	printf("\n");

	fclose(sys_conf_p);
}

void initNode(char* node_init_name, char* this_id, char* this_port)
{
	char node_init_path[150];
	memset(node_init_path, 0, sizeof(node_init_path));
	strcat(node_init_path, node_init_path_src);
	strcat(node_init_path, node_init_name);
	FILE* init_p = fopen(node_init_path, "r");			// ��ȡ�ڵ��ʼ���ļ�
	if (init_p == NULL)
	{
		printf("Error: %s open failed. Please check if the initialization file is valid.\n", node_init_name);
		exit(-1);
	}

	memset(node_list, 0, sizeof(node) * sys_set.node_num * sys_set.node_num);
	int count = 1;
	char tmp[50];
	memset(tmp, 0, sizeof(tmp));
	// �ȳ�ʼ��0,0λ��Ϊ���ڵ�
	node_list[0][0].id = this_id[0];
	node_list[0][0].distance = 0;
	node_list[0][0].neighbor = this_id[0];
	node_list[0][0].port = atoi(this_port);
	strcpy(node_list[0][0].ip, "127.0.0.1");
	while (fgets(tmp, 50, init_p) != NULL)
	{
		char* str_p;
		char delim[] = " ";
		str_p = strtok(tmp, delim);

		node_list[0][count].id = str_p[0];
		str_p = strtok(NULL, delim);
		node_list[0][count].distance = atof(str_p) > sys_set.max_distance ? INFINITE : atof(str_p);
		str_p = strtok(NULL, delim);
		node_list[0][count].port = atoi(str_p);
#ifdef IP_KEY
		str_p = strtok(NULL, delim);
		int tmp_len = strlen(str_p);
		if (str_p[tmp_len - 1] == '\n') str_p[tmp_len - 1] = '\0';
		strcpy(node_list[0][count].ip, str_p);
#else
		strcpy(node_list[count].ip, "127.0.0.1");
#endif
		node_list[0][count].neighbor = node_list[0][count].id;

		memset(tmp, 0, sizeof(tmp));
		count++;
	}
	sys_set.neibor_num = count - 1;		// �ڽӽڵ�����ֵ
	sys_set.ident_num = sys_set.neibor_num+1;	// ��ʶ��ڵ�����ֵ����ʼ��ʱ����neibor_num+1
	printf("neibor_num: %d\n", sys_set.neibor_num);
	printf("ident_num: %d\n\n", sys_set.ident_num);

	// �Բ����ڽ��б��еĽڵ���г�ʼ������ʱ����ʼ����һ�У�������·�ɱ�
	for (; count < sys_set.node_num; count++)
	{
		node_list[0][count].id = '#';		// δ֪�ڵ�id�á�#��ռλ
		node_list[0][count].distance = INFINITE;		// δ֪�ڵ������Ϊ�����
		node_list[0][count].port = 0;		// δ֪�ڵ�˿�Ϊ0����Ϊ�޷�ͨ��
		node_list[0][count].neighbor = '\0';		// δ֪�ڵ�ipΪ�գ���Ϊ�޷�ͨ��
	}
	// ��ʼ���ڽӽڵ�·������ΪINF
	for (int i = 1; i <= sys_set.neibor_num; i++)
	{
		for (int j = 0; j < sys_set.node_num; j++)
		{
			node_list[i][j].id = '#';		// δ֪�ڵ�id�á�#��ռλ
			node_list[i][j].distance = INFINITE;
			node_list[i][j].neighbor = node_list[0][i].id;
		}
	}

	printInitNode();

	fclose(init_p);
}

void initTimeSem(timemutex* tmutex, char* name)
{
	if (tmutex == NULL)
	{
		printf("tmutex is NULL\n");
		exit(-1);
	}
	tmutex->time_h = CreateSemaphore(NULL, 0, 1, name);
	if (tmutex->time_h == NULL)
	{
		printf("Error: %s create failed.\n", name);
		exit(-1);
	}
}

void initSem(HANDLE* mutex, char* name, int init_value, int max_value)
{
	mutex = CreateSemaphore(NULL, init_value, max_value, name);
	if (mutex == NULL)
	{
		printf("Error: %s create failed.\n", name);
		exit(-1);
	}
}

void printInitNode()
{
	printf("***************** Init Node List ********************\n");
	printf("DestNode\t Distance\t Neighbor\t Port\t\t IP\t\n");
	for (int j = 0; j < sys_set.node_num; j++)
	{
		printf("%c\t\t %f\t %c\t\t %d\t\t %s\t\t\n", node_list[0][j].id, node_list[0][j].distance, node_list[0][j].neighbor, node_list[0][j].port, node_list[0][j].ip);
	}
	printf("***************** END ********************\n\n");
}

void printNodeList()
{
	printf("***************** Init Node List ********************\n");
	for (int i = 0; i < sys_set.node_num; i++)
	{
		printf("DestNode, Distance, Neighbor\t");
	}
	printf("\n");
	for (int j = 0; j <= sys_set.neibor_num; j++)
	{
		for (int k = 0; k < sys_set.node_num; k++)
		{
			printf("%c\t, %f,\t %c\t", node_list[j][k].id, node_list[j][k].distance, node_list[j][k].neighbor);
		}
		printf("\n");
	}
	printf("***************** END ********************\n\n");
}