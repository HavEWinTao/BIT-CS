#pragma once
#pragma warning(disable:6031)
#pragma warning(disable:6276)
#pragma warning(disable:4996)
#include <process.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>

/******* 宏定义 *******/
#define IP_KEY			// 是否启用IP字段
#define LOG				// 是否启用LOG文件输出

/******* 结构定义 *******/
// 路由表节点信息
typedef struct Node {
	char id;				// 节点id
	float distance;	//	当前节点到该节点的距离，float型
	int port;			// UDP端口号
	char ip[25];		// IP地址
	char neighbor;	// 到达该节点的下一跳节点
} node;

typedef struct NodeSend {
	char id;
	float distance;
}nodesend;

typedef struct NodeRec{
	char rec_id;
	int rec_seq;
	nodesend list[100];	// 节点情况
} noderec;

// 时间信号量
typedef struct Timemutex {
	HANDLE time_h;	// 定时信号量句柄
	int delay;	// 周期长度
	int num;	// 接收用，指定节点号
} timemutex;

typedef struct SysSetting {
	int sys_delay;					// 传输间隔，由sys文件完成赋值
	float max_distance;		// 最大传输间距，由sys文件完成赋值
	int max_delay;				// 最大接收间隔，由sys文件完成赋值
	int node_num;				// 节点数，由sys文件完成赋值
	int neibor_num;			// 邻接节点数
	char log_save_path[120];		// log文件存储绝对路径
	int ident_num;				// 当前已识别节点数
} syssetting;


