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

/******* �궨�� *******/
#define IP_KEY			// �Ƿ�����IP�ֶ�
#define LOG				// �Ƿ�����LOG�ļ����

/******* �ṹ���� *******/
// ·�ɱ�ڵ���Ϣ
typedef struct Node {
	char id;				// �ڵ�id
	float distance;	//	��ǰ�ڵ㵽�ýڵ�ľ��룬float��
	int port;			// UDP�˿ں�
	char ip[25];		// IP��ַ
	char neighbor;	// ����ýڵ����һ���ڵ�
} node;

typedef struct NodeSend {
	char id;
	float distance;
}nodesend;

typedef struct NodeRec{
	char rec_id;
	int rec_seq;
	nodesend list[100];	// �ڵ����
} noderec;

// ʱ���ź���
typedef struct Timemutex {
	HANDLE time_h;	// ��ʱ�ź������
	int delay;	// ���ڳ���
	int num;	// �����ã�ָ���ڵ��
} timemutex;

typedef struct SysSetting {
	int sys_delay;					// ����������sys�ļ���ɸ�ֵ
	float max_distance;		// ������࣬��sys�ļ���ɸ�ֵ
	int max_delay;				// �����ռ������sys�ļ���ɸ�ֵ
	int node_num;				// �ڵ�������sys�ļ���ɸ�ֵ
	int neibor_num;			// �ڽӽڵ���
	char log_save_path[120];		// log�ļ��洢����·��
	int ident_num;				// ��ǰ��ʶ��ڵ���
} syssetting;


