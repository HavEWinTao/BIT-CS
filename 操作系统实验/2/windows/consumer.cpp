#include <iostream>
#include <conio.h>
#include <time.h>
#include <windows.h>

#include "mylibrary.h"

using namespace std;

//输出缓冲区的数据
void printBuffer(sharedMemory *addr)
{
  cout << "BUFFER: ";
  //缓冲区为空
  if (addr->data.ISEMPTY == TRUE)
  {
    cout << "EMPTY";
  }
  else
  {
    int i;
    //找下标
    if (addr->data.TAIL - 1 >= addr->data.HEAD)
    {
      i = addr->data.TAIL - 1;
    }
    else
    {
      i = addr->data.TAIL - 1 + BUFFER_NUM;
    }
    //从前到后遍历 buffer 内容并输出
    for (; i >= addr->data.HEAD; i--)
    {
      cout << addr->data.Buffer[i % BUFFER_NUM] << " ";
    }
  }
  cout << endl;
}

int main(int argc, char const *argv[])
{
  srand(GetCurrentProcessId()); //随机种子

  //创建文件映射对象
  HANDLE SharedFileHandle;
  SharedFileHandle = OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, SHM_NAME);

  //将文件映射到进程的地址空间
  LPVOID FilePointer;
  FilePointer = MapViewOfFile(SharedFileHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);

  //共享缓存区
  sharedMemory *SharedMemory = (sharedMemory *)FilePointer;
  //获取三个信号量的句柄
  HANDLE producerSemEmpty = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, SEM_EMPTY);
  HANDLE producerSemFull = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, SEM_FULL);
  HANDLE producerMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, SEM_MUTEX);

  //消费者多次消费
  for (int i = 1; i <= CONSUME_NUM; i++)
  {
    int start = time(NULL); //记录开始时间
    //随机一个等待时间
    Sleep((rand() % 2 + 1) * 1000);

    //两个P操作，先申请FULL，再申请MUTEX
    WaitForSingleObject(producerSemFull, INFINITE);
    WaitForSingleObject(producerMutex, INFINITE);

    //取出队列头部数据
    char *stock = SharedMemory->data.Buffer[SharedMemory->data.HEAD];
    //将头部指针head向前移动一位
    SharedMemory->data.HEAD = (SharedMemory->data.HEAD + 1) % BUFFER_NUM;
    //判断buffer是否为空
    if (SharedMemory->data.HEAD == SharedMemory->data.TAIL)
    {
      SharedMemory->data.ISEMPTY = TRUE;
    }

    int end = time(NULL); //记录结束时间

    //输出本次操作内容
    cout << "consumer" << i << " consumes: " << stock << " cost time: " << end - start << endl;
    //输出buffer内容
    printBuffer(SharedMemory);

    //两个V操作，先释放MUTEX，再释放EMPTY
    ReleaseMutex(producerMutex);
    ReleaseSemaphore(producerSemEmpty, 1, NULL);
  }

  //解除进程对一个文件映射对象的映射
  UnmapViewOfFile(FilePointer);
  FilePointer = NULL;

  //关闭文件映射句柄
  CloseHandle(SharedFileHandle);

  return 0;
}
