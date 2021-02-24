#include <windows.h>
#include <psapi.h>
#include <iomanip>
#include <iostream>
#include <tlhelp32.h>
#include <string.h>
#include <shlwapi.h>
#include <conio.h>

//单位
#define KB 1024
#define MB (1024 * 1024)
#define GB (1024 * 1024 * 1024)

using namespace std;

//引入库
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "shlwapi.lib")

void Memory()
{
	//获得当前内存状态的信息
	MEMORYSTATUSEX MemoryStatus;
	MemoryStatus.dwLength = sizeof(MemoryStatus);
	GlobalMemoryStatusEx(&MemoryStatus);
	//输出当前内存状态
	cout << "-----GlobalMemoryStatusEx-----" << endl;
	cout << "ullTotalPhys: "; //物理内存总量
	cout << fixed << setprecision(2) << (float)MemoryStatus.ullTotalPhys / GB << "GB" << endl;
	cout << "dwMemoryLoad: "; //已用内存百分比
	cout << fixed << setprecision(2) << (float)MemoryStatus.dwMemoryLoad << "%" << endl;
	cout << "ullAvailPhys: "; //可用物理内存
	cout << fixed << setprecision(2) << (float)MemoryStatus.ullAvailPhys / GB << "GB" << endl;
	cout << "ullTotalPageFile: "; //页交换文件最多能放的字节数
	cout << fixed << setprecision(2) << (float)MemoryStatus.ullTotalPageFile / GB << "GB" << endl;
	cout << "ullAvailPageFile: "; //页交换文件中尚未分配给进程的字节数
	cout << fixed << setprecision(2) << (float)MemoryStatus.ullAvailPageFile / GB << "GB" << endl;
	cout << "ullTotalVirtual: "; //用户区总的虚拟地址空间
	cout << fixed << setprecision(2) << (float)MemoryStatus.ullTotalVirtual / GB << "GB" << endl;
	cout << "ullAvailVirtual: "; //用户区当前可用的虚拟地址空间
	cout << fixed << setprecision(2) << (float)MemoryStatus.ullAvailVirtual / GB << "GB" << endl;
	cout << endl;
}

void System(SYSTEM_INFO SystemInfo)
{
	//获取当前系统的信息
	ZeroMemory(&SystemInfo, sizeof(SystemInfo));
	GetSystemInfo(&SystemInfo);
	//输出当前系统的信息
	cout << "-----SystemInfo-----" << endl;
	cout << "dwNumberOfProcessors: "; //处理器个数
	cout << SystemInfo.dwNumberOfProcessors << endl;
	cout << "dwPageSize: "; //处理器分页大小
	cout << SystemInfo.dwPageSize / KB << " KB" << endl;
	cout << "dwProcessorType: "; //处理器类型
	cout << SystemInfo.dwProcessorType << endl;
	cout << "lpMaximumApplicationAddress: "; //最大寻址单元
	cout << SystemInfo.lpMaximumApplicationAddress << endl;
	cout << "lpMinimumApplicationAddress: "; //最小寻址单元
	cout << SystemInfo.lpMinimumApplicationAddress << endl;
	cout << "Allocation granularity: "; //虚拟内存空间的粒度
	cout << SystemInfo.dwAllocationGranularity / KB << " KB" << endl;
	cout << endl;
}

void Performance()
{
	//获取系统的存储器使用情况
	PERFORMANCE_INFORMATION PerformanceInfo;
	PerformanceInfo.cb = sizeof(PerformanceInfo);
	GetPerformanceInfo(&PerformanceInfo, sizeof(PerformanceInfo));
	//输出当前存储器的使用情况
	cout << "-----PerformanceInfo-----" << endl;
	cout << "SystemCache: "; //系统Cache的容量
	cout << PerformanceInfo.SystemCache << endl;
	cout << "PageSize: "; //页的大小
	cout << PerformanceInfo.PageSize << endl;
	cout << "CommitTotal/CommitLimit: "; //系统提交的页面总数 系统可提交的最大页面总数
	cout << PerformanceInfo.CommitTotal << "/" << PerformanceInfo.CommitLimit << endl;
	cout << "CommitPeak: "; //系统历史提交页面峰值
	cout << PerformanceInfo.CommitPeak << endl;
	cout << "PhysicalAvailable/PhysicalTotal: "; //可用的物理内存 总物理内存（按页）
	cout << PerformanceInfo.PhysicalAvailable << "/" << PerformanceInfo.PhysicalTotal << " pages" << endl;
	cout << "HandleCount: "; //打开的句柄个数
	cout << PerformanceInfo.HandleCount << endl;
	cout << "ProcessCount: "; //进程个数
	cout << PerformanceInfo.ProcessCount << endl;
	cout << "ThreadCount: "; //线程个数
	cout << PerformanceInfo.ThreadCount << endl;
	cout << endl;
}

int main(int argc, char *argv[])
{
	SYSTEM_INFO SystemInfo; //系统信息

	//显示内存信息
	Memory();
	//显示系统信息
	System(SystemInfo);
	//显示存储器信息
	Performance();

	PROCESSENTRY32 pe; //进程信息
	pe.dwSize = sizeof(pe);
	//为进程建立快照
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	//获取当前进程的快照
	BOOL bMore = ::Process32First(hProcessSnap, &pe);

	//如果有要查看的进程
	char DetailProcess[260];
	sprintf(DetailProcess, "%s", argv[1]);
	DWORD pid = 0; //记录该进程的PID

	//当前进程的名字
	char ProcessName[260];

	cout << "-----PROCESS INFO-----" << endl;

	//统计并记录总数
	PROCESS_MEMORY_COUNTERS pmc;
	while (bMore)
	{
		//打开一个已存在的进程对象
		HANDLE hP = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
		ZeroMemory(&pmc, sizeof(pmc));

		//获取内存的使用情况
		GetProcessMemoryInfo(hP, &pmc, sizeof(pmc));

		//输出相关的信息
		cout << "ProcessID:" << pe.th32ProcessID << "    ExeFile:" << pe.szExeFile << "    WorkingSetSize:" << pmc.WorkingSetSize / MB << "MB" << endl;

		//判断和要查看的进程是否相等
		sprintf(ProcessName, "%s", pe.szExeFile);
		if (!strcmp(ProcessName, DetailProcess))
		{
			pid = pe.th32ProcessID; //相等则记录PID
		}

		bMore = ::Process32Next(hProcessSnap, &pe);
	}
	cout << endl;

	//如果有要查看的进程
	if (argc == 2)
	{
		cout << "-----Process <" << DetailProcess << "> detail-----" << endl;

		//打开一个已存在的进程对象
		HANDLE hP = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

		//进程虚拟内存空间的基本信息结构
		MEMORY_BASIC_INFORMATION mbi;
		ZeroMemory(&mbi, sizeof(mbi));

		//虚拟空间的最小地址
		LPCVOID pStart = (LPVOID)SystemInfo.lpMinimumApplicationAddress;
		//while (pStart < SystemInfo.lpMaximumApplicationAddress)
		for (int i = 0; i < 10; i++)
		{
			//查询地址空间中内存地址的信息
			if (VirtualQueryEx(hP, pStart, &mbi, sizeof(mbi)) == sizeof(mbi))
			{
				//区域大小
				LPCVOID pEnd = (PBYTE)pStart + mbi.RegionSize;
				TCHAR szSize[MAX_PATH];
				StrFormatByteSize(mbi.RegionSize, szSize, MAX_PATH);
				cout << "Block Address:" << pStart << "-" << pEnd << "(" << szSize << ")    ";

				//状态
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

				//权限
				cout << "    Protection:" << mbi.Protect;

				//种类
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
