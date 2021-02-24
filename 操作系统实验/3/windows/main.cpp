#include <windows.h>
#include <psapi.h>
#include <iomanip>
#include <iostream>
#include <tlhelp32.h>
#include <string.h>
#include <shlwapi.h>
#include <conio.h>

//��λ
#define KB 1024
#define MB (1024 * 1024)
#define GB (1024 * 1024 * 1024)

using namespace std;

//�����
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "shlwapi.lib")

void Memory()
{
	//��õ�ǰ�ڴ�״̬����Ϣ
	MEMORYSTATUSEX MemoryStatus;
	MemoryStatus.dwLength = sizeof(MemoryStatus);
	GlobalMemoryStatusEx(&MemoryStatus);
	//�����ǰ�ڴ�״̬
	cout << "-----GlobalMemoryStatusEx-----" << endl;
	cout << "ullTotalPhys: "; //�����ڴ�����
	cout << fixed << setprecision(2) << (float)MemoryStatus.ullTotalPhys / GB << "GB" << endl;
	cout << "dwMemoryLoad: "; //�����ڴ�ٷֱ�
	cout << fixed << setprecision(2) << (float)MemoryStatus.dwMemoryLoad << "%" << endl;
	cout << "ullAvailPhys: "; //���������ڴ�
	cout << fixed << setprecision(2) << (float)MemoryStatus.ullAvailPhys / GB << "GB" << endl;
	cout << "ullTotalPageFile: "; //ҳ�����ļ�����ܷŵ��ֽ���
	cout << fixed << setprecision(2) << (float)MemoryStatus.ullTotalPageFile / GB << "GB" << endl;
	cout << "ullAvailPageFile: "; //ҳ�����ļ�����δ��������̵��ֽ���
	cout << fixed << setprecision(2) << (float)MemoryStatus.ullAvailPageFile / GB << "GB" << endl;
	cout << "ullTotalVirtual: "; //�û����ܵ������ַ�ռ�
	cout << fixed << setprecision(2) << (float)MemoryStatus.ullTotalVirtual / GB << "GB" << endl;
	cout << "ullAvailVirtual: "; //�û�����ǰ���õ������ַ�ռ�
	cout << fixed << setprecision(2) << (float)MemoryStatus.ullAvailVirtual / GB << "GB" << endl;
	cout << endl;
}

void System(SYSTEM_INFO SystemInfo)
{
	//��ȡ��ǰϵͳ����Ϣ
	ZeroMemory(&SystemInfo, sizeof(SystemInfo));
	GetSystemInfo(&SystemInfo);
	//�����ǰϵͳ����Ϣ
	cout << "-----SystemInfo-----" << endl;
	cout << "dwNumberOfProcessors: "; //����������
	cout << SystemInfo.dwNumberOfProcessors << endl;
	cout << "dwPageSize: "; //��������ҳ��С
	cout << SystemInfo.dwPageSize / KB << " KB" << endl;
	cout << "dwProcessorType: "; //����������
	cout << SystemInfo.dwProcessorType << endl;
	cout << "lpMaximumApplicationAddress: "; //���Ѱַ��Ԫ
	cout << SystemInfo.lpMaximumApplicationAddress << endl;
	cout << "lpMinimumApplicationAddress: "; //��СѰַ��Ԫ
	cout << SystemInfo.lpMinimumApplicationAddress << endl;
	cout << "Allocation granularity: "; //�����ڴ�ռ������
	cout << SystemInfo.dwAllocationGranularity / KB << " KB" << endl;
	cout << endl;
}

void Performance()
{
	//��ȡϵͳ�Ĵ洢��ʹ�����
	PERFORMANCE_INFORMATION PerformanceInfo;
	PerformanceInfo.cb = sizeof(PerformanceInfo);
	GetPerformanceInfo(&PerformanceInfo, sizeof(PerformanceInfo));
	//�����ǰ�洢����ʹ�����
	cout << "-----PerformanceInfo-----" << endl;
	cout << "SystemCache: "; //ϵͳCache������
	cout << PerformanceInfo.SystemCache << endl;
	cout << "PageSize: "; //ҳ�Ĵ�С
	cout << PerformanceInfo.PageSize << endl;
	cout << "CommitTotal/CommitLimit: "; //ϵͳ�ύ��ҳ������ ϵͳ���ύ�����ҳ������
	cout << PerformanceInfo.CommitTotal << "/" << PerformanceInfo.CommitLimit << endl;
	cout << "CommitPeak: "; //ϵͳ��ʷ�ύҳ���ֵ
	cout << PerformanceInfo.CommitPeak << endl;
	cout << "PhysicalAvailable/PhysicalTotal: "; //���õ������ڴ� �������ڴ棨��ҳ��
	cout << PerformanceInfo.PhysicalAvailable << "/" << PerformanceInfo.PhysicalTotal << " pages" << endl;
	cout << "HandleCount: "; //�򿪵ľ������
	cout << PerformanceInfo.HandleCount << endl;
	cout << "ProcessCount: "; //���̸���
	cout << PerformanceInfo.ProcessCount << endl;
	cout << "ThreadCount: "; //�̸߳���
	cout << PerformanceInfo.ThreadCount << endl;
	cout << endl;
}

int main(int argc, char *argv[])
{
	SYSTEM_INFO SystemInfo; //ϵͳ��Ϣ

	//��ʾ�ڴ���Ϣ
	Memory();
	//��ʾϵͳ��Ϣ
	System(SystemInfo);
	//��ʾ�洢����Ϣ
	Performance();

	PROCESSENTRY32 pe; //������Ϣ
	pe.dwSize = sizeof(pe);
	//Ϊ���̽�������
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	//��ȡ��ǰ���̵Ŀ���
	BOOL bMore = ::Process32First(hProcessSnap, &pe);

	//�����Ҫ�鿴�Ľ���
	char DetailProcess[260];
	sprintf(DetailProcess, "%s", argv[1]);
	DWORD pid = 0; //��¼�ý��̵�PID

	//��ǰ���̵�����
	char ProcessName[260];

	cout << "-----PROCESS INFO-----" << endl;

	//ͳ�Ʋ���¼����
	PROCESS_MEMORY_COUNTERS pmc;
	while (bMore)
	{
		//��һ���Ѵ��ڵĽ��̶���
		HANDLE hP = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
		ZeroMemory(&pmc, sizeof(pmc));

		//��ȡ�ڴ��ʹ�����
		GetProcessMemoryInfo(hP, &pmc, sizeof(pmc));

		//�����ص���Ϣ
		cout << "ProcessID:" << pe.th32ProcessID << "    ExeFile:" << pe.szExeFile << "    WorkingSetSize:" << pmc.WorkingSetSize / MB << "MB" << endl;

		//�жϺ�Ҫ�鿴�Ľ����Ƿ����
		sprintf(ProcessName, "%s", pe.szExeFile);
		if (!strcmp(ProcessName, DetailProcess))
		{
			pid = pe.th32ProcessID; //������¼PID
		}

		bMore = ::Process32Next(hProcessSnap, &pe);
	}
	cout << endl;

	//�����Ҫ�鿴�Ľ���
	if (argc == 2)
	{
		cout << "-----Process <" << DetailProcess << "> detail-----" << endl;

		//��һ���Ѵ��ڵĽ��̶���
		HANDLE hP = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

		//���������ڴ�ռ�Ļ�����Ϣ�ṹ
		MEMORY_BASIC_INFORMATION mbi;
		ZeroMemory(&mbi, sizeof(mbi));

		//����ռ����С��ַ
		LPCVOID pStart = (LPVOID)SystemInfo.lpMinimumApplicationAddress;
		//while (pStart < SystemInfo.lpMaximumApplicationAddress)
		for (int i = 0; i < 10; i++)
		{
			//��ѯ��ַ�ռ����ڴ��ַ����Ϣ
			if (VirtualQueryEx(hP, pStart, &mbi, sizeof(mbi)) == sizeof(mbi))
			{
				//�����С
				LPCVOID pEnd = (PBYTE)pStart + mbi.RegionSize;
				TCHAR szSize[MAX_PATH];
				StrFormatByteSize(mbi.RegionSize, szSize, MAX_PATH);
				cout << "Block Address:" << pStart << "-" << pEnd << "(" << szSize << ")    ";

				//״̬
				cout << "Block Stat:";
				switch (mbi.State)
				{
				case MEM_COMMIT:
					cout << "Commited";
					break;
				case MEM_FREE:
					cout << "Free";
					break;
				case MEM_RESERVE:
					cout << "Reserved";
					break;
				default:
					cout << "Unknown";
				}

				//Ȩ��
				cout << "    Protection:" << mbi.Protect;

				//����
				cout << "    Type:";
				switch (mbi.Type)
				{
				case MEM_IMAGE:
					cout << "Image";
					break;
				case MEM_MAPPED:
					cout << "Mapped";
					break;
				case MEM_PRIVATE:
					cout << "Private";
					break;
				default:
					cout << "Unknown";
				}

				pStart = pEnd;
			}
			cout << endl;
		}
	}

	return 0;
}
