#include <windows.h>
#include <tchar.h>
#include <conio.h>
#include <time.h>
#include <iostream>

#include "mylibrary.h"

using namespace std;

int main(int argc, char const *argv[])
{
  //主进程开始
  cout << "main process start" << endl;

  //创建文件映射对象
  HANDLE SharedFileHandle;
  SharedFileHandle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, SHARED_MEM_SIZE, SHM_NAME);

  //将文件映射到进程的地址空间
  LPVOID FilePointer;
  FilePointer = MapViewOfFile(SharedFileHandle, FILE_MAP_ALL_ACCESS, 0, 0, SHARED_MEM_SIZE);

  //共享缓存区
  sharedMemory *pcpSharedMemory = (sharedMemory *)FilePointer;
  //初始化共享缓存区
  pcpSharedMemory->data.HEAD = 0;
  pcpSharedMemory->data.TAIL = 0;

  //创建信号量，两个同步信号量一个互斥信号量
  //同步信号量 EMPTY = BUFFER_SIZE，表示可以进入缓冲区
  HANDLE semEmpty = CreateSemaphore(NULL, BUFFER_NUM, BUFFER_NUM, SEM_EMPTY);
  //同步信号量 FULL = 0，表示当前缓冲区没有物品
  HANDLE semFull = CreateSemaphore(NULL, 0, BUFFER_NUM, SEM_FULL);
  //互斥信号量 MUTEX = 1
  HANDLE semMutex = CreateMutex(NULL, FALSE, SEM_MUTEX);

  //解除进程对一个文件映射对象的映射
  UnmapViewOfFile(FilePointer);
  FilePointer = NULL;

  //生产者消费者子进程的句柄集合
  HANDLE createdProcessHandles[TOTAL_PROCESS_COUNT + 1];

  //创建2个生产者进程
  for (int ProcessId = 1; ProcessId <= PRODUCER_PROCESS_NUM; ProcessId++)
  {
    //进程的启动信息结构体
    STARTUPINFO si;
    //进程的创建信息结构体
    PROCESS_INFORMATION pi;
    //初始化各个结构体
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    //获得创建进程返回值，对异常进行处理
    if (!CreateProcess(PRODUCER_PATH, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
      cout << "Create Failed!" << endl;
      exit(1);
    }
    createdProcessHandles[ProcessId] = pi.hProcess;

    //输出创建的进程信息
    cout << "create producer" << ProcessId << endl;
  }

  //创建3个消费者进程
  for (int ProcessId = 1; ProcessId <= CONSUMER_PROCESS_NUM; ProcessId++)
  {
    //进程的启动信息结构体
    STARTUPINFO si;
    //进程的创建信息结构体
    PROCESS_INFORMATION pi;
    //初始化各个结构体
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    //获得创建进程返回值，对异常进行处理
    if (!CreateProcess(CONSUMER_PATH, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
      cout << "Create Failed!" << endl;
      exit(1);
    }
    createdProcessHandles[ProcessId + PRODUCER_PROCESS_NUM] = pi.hProcess;
    //输出创建的进程信息
    cout << "create consumer" << ProcessId << endl;
  }

  //主进程依次等待5个子进程返回
  for (int i = 1; i <= TOTAL_PROCESS_COUNT; i++)
  {
    //等待子进程返回
    WaitForSingleObject(createdProcessHandles[i], INFINITE);
    //关闭进程的句柄
    CloseHandle(createdProcessHandles[i]);
  }

  //关闭共享映射文件句柄
  CloseHandle(SharedFileHandle);

  cout << "main process end" << endl;

  return 0;
}
