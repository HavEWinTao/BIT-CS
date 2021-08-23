#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include "config.h"

/******* 函数声明 *******/
void initSys(char* node_id);																										// 节点系统配置初始化函数，参数为系统配置文件指针
void initNode(char* node_init_name, char* this_id, char* this_port);					// 节点初始化函数，参数为初始化文件指针
void initTimeSem(timemutex* tmutex, char* name);														// 初始化时间信号量
void initSem(HANDLE* mutex, char* name, int init_value, int max_value);					//	初始化普通信号量

DWORD WINAPI sendTimerFunc(LPVOID lpParam);													// 发送定时器线程
DWORD WINAPI recTimerFunc(LPVOID lpParam);													// 接收定时器线程
DWORD WINAPI recThreadFunc(LPVOID lpParam);											// 接收子线程执行函数，用于接收邻接节点的交换信息
DWORD WINAPI sendThreadFunc(LPVOID lpParam);											// 发送子线程执行函数

void sendPart();
void receivePart();
void closeAll();
void parseAll();
void printInitNode();
void printNodeList();
int dv(noderec* node_for_rec, int neibor_num);			// 接收路由表更新

/********* 全局变量声明 *********/
const char sys_init_path[100] = "E:\\Study\\3rd_University_Down\\ComputerNetwork\\Experiment\\实验四 小组作业2\\Code\\sys.txt";
const char node_init_path_src[100] = "E:\\Study\\3rd_University_Down\\ComputerNetwork\\Experiment\\实验四 小组作业2\\Code\\node_init\\";
const char log_save_path_src[100] = "E:\\Study\\3rd_University_Down\\ComputerNetwork\\Experiment\\实验四 小组作业2\\Code\\log\\";
char log_save_path[120];
syssetting sys_set;					// 系统参数配置
HANDLE mutex;					// 路由表读写互斥量
HANDLE st_h;
timemutex stm;					
HANDLE rt_h;
HANDLE send_h;
HANDLE rec_h;
HANDLE nt_h;
timemutex ntm;
HANDLE rec_mutex;		// 接收标志信号量
static int time_lim[100] = { 0 };		// 接收标志，0为当前周期未接收，1为当前周期已接收
static int rec_flag[100] = { 0 };		// 接收节点健康标志，0为健康，1为宕机
HANDLE nrec_h;
static node node_list[100][100];				// 路由表，默认设置最大端点数为100，竖为所有邻接节点，横为所有节点
static int flag = 1;		// 所有线程退出标志，1运行，0退出，初始为1
static int seq = 0;		// sequence


int main(int argc, char* argv[])
{
	// 命令参数错误，终止并报错
	if (argc != 4)
	{
		printf("Command values error: please call the command in the format of \"Programe_Name Node_id Node_UDP_Port Initialization_File_Name \".\n");
		exit(-1);
	}
	initSys(argv[1]);													//初始化节点系统配置
	initNode(argv[3], argv[1], argv[2]);			// 初始化节点，包含邻接节点与初始路由表
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
		if (key_cmd == 'K')		// 跳出然后自杀
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
			ReleaseSemaphore(tm.time_h, 1, NULL);	// 发送
		}
	}
	return 0;
}

DWORD WINAPI recTimerFunc(LPVOID lpParam)
{
	// 恢复时首先清空数组状况
	memset(time_lim, 0, sizeof(time_lim));
	memset(rec_flag, 0, sizeof(rec_flag));
	timemutex tm = *(timemutex*)lpParam;
	time_t start[100];	// 存储所有邻接节点的上一次接收时间
	time_t current = 0;
	for (int i = 1; i <= sys_set.neibor_num; i++)
	{
		start[i] = clock();
	}
	printf("** REC Timer: start init finished.\n");
	while (flag)
	{
		current = clock();
		// 每次循环检查所有节点的接收状况
		for (int i = 1; i <= sys_set.neibor_num; i++)
		{
			if (time_lim[i] == 0)	// 序号为i的邻接节点当前未接收
			{
				if (current - start[i] >= sys_set.max_delay && rec_flag[i] != 1)	// 已超时且未置不健康
				{
					WaitForSingleObject(rec_mutex, INFINITE);
					rec_flag[i] = 1;	// 将指定id节点置为不健康
					ReleaseSemaphore(rec_mutex, 1, NULL);
					// printf("REC Timer: 已将%d节点置为不健康，标志为 %d\n", i, rec_flag[i]);
				}
			}
			else	// 已接收，重置标志及i节点对应的上一次时间
			{
				WaitForSingleObject(rec_mutex, INFINITE);
				time_lim[i] = 0;		// 重置接收标志
				ReleaseSemaphore(rec_mutex, 1, NULL);
				start[i] = clock();
			}
		}
	}
	return 0;
}

DWORD WINAPI recThreadFunc(LPVOID lpParam)
{
	int node_i = 0;	// 此时接收的邻接节点在本地路由表的序号位置
	int rec_seq = 0;	// 接收帧序号

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
		// 循环检查所有邻接节点的健康状况
		WaitForSingleObject(rec_mutex, INFINITE);
		for (int i = 1; i <= sys_set.neibor_num; i++)
		{
			// 在此处仅更新不健康节点，健康节点将在所有节点完成检查后成为待接收态，监听端口
			if (rec_flag[i] == 1 && node_list[0][i].distance < INFINITE)	// 该节点不健康且未更新为不可达
			{
				printf("node %d, id=%c, unhealthy.\n", i, node_list[0][i].id);
				WaitForSingleObject(mutex, INFINITE);	// 路由表锁
				node_list[0][i].distance = INFINITE;
				//// 手动更新该邻接节点的cost状况
				//for (int j = 0; j < sys_set.node_num; j++)
				//{
				//	node_list[i][j].distance = INFINITE;		// 该节点的横行全部置INF
				//}
				//// 之后更新本地路由表中所有采用了该节点作为下一跳的节点信息，需要在其对应列找到最小的替换
				//for (int m = 1; m < sys_set.node_num; m++)
				//{
				//	if (node_list[0][m].neighbor == node_list[0][i].id)	// 找到下一跳是该失效节点的节点了
				//	{
				//		node_list[0][m].distance = INFINITE;	// 首先将该节点置INF
				//		for (int k = 1; k <= sys_set.neibor_num; k++)	// 然后遍历所有其他邻接节点到该节点的cost是否有更优，循环以寻找最优
				//		{
				//			// 下一跳不是该节点
				//			if (k != i)
				//			{
				//				// 距离和小于当前（INF）
				//				if (node_list[k][m].distance + node_list[0][k].distance < node_list[0][m].distance)
				//				{
				//					// 小于则更新distance和neibor信息
				//					node_list[0][m].distance = node_list[k][m].distance + node_list[0][k].distance;
				//					node_list[0][m].neighbor = node_list[0][k].id;
				//				}
				//			}
				//		}
				//	}
				//}
				int change = 1;
				// 此处也可以触发发送线程，暂不做
				ReleaseSemaphore(mutex, 1, NULL);	// 释放路由表锁
			}
		}
		ReleaseSemaphore(rec_mutex, 1, NULL);

		// 检查完毕，开始接收新信息
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
			WaitForSingleObject(mutex, INFINITE);	// 接收到东西了才开始申请路由表锁
			for (int m = 1; m <= sys_set.neibor_num; m++)		// 找本次接收到的邻接节点的对应位置
			{
				if (node_list[0][m].id == node_for_rec->rec_id)	
				{
					node_i = m;
					rec_seq = node_for_rec->rec_seq;
					printf("rec_seq: %d\n",rec_seq);
					WaitForSingleObject(rec_mutex, INFINITE);
					time_lim[m] = 1;	// 对接收标志更新
					rec_flag[m] = 0;	// 如果该节点刚好是失效节点，则说明已恢复，将其重置为0-健康
					ReleaseSemaphore(rec_mutex, 1, NULL);
				}
			}
			int change = dv(node_for_rec, node_i);		// 路由表是否有更新
			// 此处可以根据change增加sendmutex来触发发送线程，暂不做
			
#ifdef LOG
			FILE* log_save_p = fopen(sys_set.log_save_path, "a");		// 打开log文件
			if (log_save_p == NULL)
			{
				printf("Error: log_save_p open failed.\n");
				exit(-1);
			}
			char log_str[1000];
			char int_str[35];
			memset(log_str, 0, sizeof(log_str));
			memset(int_str, 0, sizeof(int_str));

			time_t save_time;		// 获取当前时间的字符串
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
				strcat(log_str, &node_list[0][node_i].id);	// 接收到的所有neibor都是该邻接节点
				strcat(log_str, "\n");
			}
			strcat(log_str, "\n");
			printf("%s", log_str);
			fputs(log_str, log_save_p);	// 将log写入log文件
			fclose(log_save_p);
#endif
			ReleaseSemaphore(mutex, 1, NULL);	// 释放路由表锁
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
		WaitForSingleObject(stm.time_h, INFINITE);	// 优先时间锁
		WaitForSingleObject(mutex, INFINITE);	// 其次路由表锁

		FILE* log_save_p = fopen(sys_set.log_save_path, "a");		// 打开log文件
		if (log_save_p == NULL)
		{
			printf("Error: log_save_p open failed.\n");
			exit(-1);
		}

		noderec *node_for_send = (noderec*)malloc(sizeof(noderec));			// 发送用路由表项
		node_for_send->rec_id = node_list[0][0].id;
		node_for_send->rec_seq = seq;
		for (int i = 0; i < sys_set.node_num; i++)	// 打包路由表，包含所有节点信息
		{
			node_for_send->list[i].id = node_list[0][i].id;
			node_for_send->list[i].distance = node_list[0][i].distance;
		}

		for (int i = 1; i <= sys_set.neibor_num; i++)	// 向邻接节点发送路由表，只有邻接节点
		{
			if (node_list[0][i].distance >= sys_set.max_distance) continue;	// 跳过不可达节点，该节点不发送消息
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

			time_t save_time;		// 获取当前时间的字符串
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
			fputs(log_str, log_save_p);	// 将log写入log文件
#endif
			free(buffer);
			seq++;
		}
		free(node_for_send);
		fclose(log_save_p);	// 关闭log文件
		ReleaseSemaphore(mutex, 1, NULL);	// 释放路由表锁
	}
	return 0;
}

void sendPart()
{
	// 创建初始化时钟线程
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
	
	// 创建发送线程
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
	// 初始化接收信号量
	initSem(&rec_mutex, "RECMUTEX", 1, 1);
	// 创建初始化时钟线程
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

	// 创建接收线程
	DWORD nrec_h_id;
	nrec_h = CreateThread(NULL, 0, recThreadFunc, NULL, 0, &nrec_h_id);	// 传入指定节点信息
	if (nrec_h == NULL)
	{
		printf("Error: nrec_h create failed.\n");
		exit(-1);
	}
}

void closeAll()
{
	//WaitForSingleObject(st_h, INFINITE);	//	等待线程结束
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
	WaitForSingleObject(st_h, INFINITE);	//	强制退出定时器
	WaitForSingleObject(send_h, INFINITE);
	WaitForSingleObject(rec_h, INFINITE);
	WaitForSingleObject(nt_h, INFINITE);
}

int dv(noderec *node_for_rec, int neibor_num)
{
	int change = 0;	// 路由表是否更新标志

	printf("DV前：\n");
	printNodeList();

	// 首先将收到的信息整合到本地路由矩阵中
	for (int i = 0; i < sys_set.node_num; i++)
	{
		// 在本地路由表中已识别的部分进行查询，设置find_flag标志表示是否找到
		if (node_for_rec->list[i].id == '#') continue;		// 路由向量中该项不存在，不考虑
		int find_flag = 0;
		for (int k = 0; k < sys_set.ident_num; k++)
		{
			if (node_list[0][k].id == node_for_rec->list[i].id)	// 找到了
			{
				find_flag = 1;
				node_list[neibor_num][k].id = node_for_rec->list[i].id;
				node_list[neibor_num][k].distance = node_for_rec->list[i].distance;
			}
		}
		if (find_flag == 0)	// 没找到
		{
			// 创建新节点信息，并进行赋值
			node_list[0][sys_set.ident_num].id = node_for_rec->list[i].id;
			node_list[0][sys_set.ident_num].distance = node_for_rec->list[i].distance + node_list[neibor_num][0].distance;
			node_list[0][sys_set.ident_num].neighbor = node_for_rec->list[i].id;
			node_list[neibor_num][sys_set.ident_num].id = node_for_rec->list[i].id;
			node_list[neibor_num][sys_set.ident_num].distance = node_for_rec->list[i].distance;
			sys_set.ident_num++;
		}
	}
	printf("DV中前\n");
	printNodeList();

	//// 新节点信息已经全部收容，更新本地路由表，注意此处必定更新为邻接节点之一
	//// 对所有下一跳为本次接收到的节点的节点进行更新
	//for (int i = 1; i < sys_set.node_num; i++)
	//{
	//	if (node_list[0][i].neighbor == node_list[0][neibor_num].id)
	//	{
	//		node_list[0][i].distance = node_list[0][neibor_num].distance + node_list[neibor_num][i].distance;
	//	}
	//}

	// 新节点信息已经全部收容，更新本地路由表，注意此处必定更新为邻接节点之一
	// 更新所有节点
	// 提前更新该接收节点
	node_list[0][neibor_num].distance = node_list[neibor_num][0].distance;
	for (int i = 1; i <= sys_set.node_num; i++)
	{
		if (node_list[neibor_num][i].id == '#') continue;	// 路由向量中该节点无信息，直接跳过
		if (node_list[0][i].neighbor == node_list[0][neibor_num].id)		// 下一跳为当前接收的节点
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
	printf("DV中后\n");
	printNodeList();


	// 规范化
	for (int i = 0; i < sys_set.node_num; i++)
	{
		if (node_list[0][i].distance > sys_set.max_distance)
		{
			node_list[0][i].distance = INFINITE;
		}
	}
	printf("DV后\n");
	printNodeList();


	//for (int i = 1; i < sys_set.node_num; i++)
	//{
	//	// 不再覆盖更新本邻接节点的数据
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
	FILE* sys_conf_p = fopen(sys_init_path, "r");		// 读取系统配置文件
	if (sys_conf_p == NULL)
	{
		printf("Error: sys_conf_p open failed.\n");
		exit(-1);
	}

	// 按行读取，strtok按":"分割，对应识别并赋值
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
	// 拼接log存储路径
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
	FILE* init_p = fopen(node_init_path, "r");			// 读取节点初始化文件
	if (init_p == NULL)
	{
		printf("Error: %s open failed. Please check if the initialization file is valid.\n", node_init_name);
		exit(-1);
	}

	memset(node_list, 0, sizeof(node) * sys_set.node_num * sys_set.node_num);
	int count = 1;
	char tmp[50];
	memset(tmp, 0, sizeof(tmp));
	// 先初始化0,0位置为本节点
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
	sys_set.neibor_num = count - 1;		// 邻接节点数赋值
	sys_set.ident_num = sys_set.neibor_num+1;	// 已识别节点数赋值，初始化时等于neibor_num+1
	printf("neibor_num: %d\n", sys_set.neibor_num);
	printf("ident_num: %d\n\n", sys_set.ident_num);

	// 对不在邻接列表中的节点进行初始化，此时仅初始化第一行，即本地路由表
	for (; count < sys_set.node_num; count++)
	{
		node_list[0][count].id = '#';		// 未知节点id用‘#’占位
		node_list[0][count].distance = INFINITE;		// 未知节点距离设为无穷大
		node_list[0][count].port = 0;		// 未知节点端口为0，因为无法通信
		node_list[0][count].neighbor = '\0';		// 未知节点ip为空，因为无法通信
	}
	// 初始化邻接节点路由向量为INF
	for (int i = 1; i <= sys_set.neibor_num; i++)
	{
		for (int j = 0; j < sys_set.node_num; j++)
		{
			node_list[i][j].id = '#';		// 未知节点id用‘#’占位
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