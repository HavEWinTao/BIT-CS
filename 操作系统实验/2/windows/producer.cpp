#include <iostream>
#include <conio.h>
#include <time.h>
#include <stdlib.h>
#include <windows.h>

#include "mylibrary.h"

using namespace std;

//可能的数据
char stored_data[][10] = {"AAAAAAAAA",
                          "BBBBBBBB",
                          "CCCCCCCCC",
                          "DDDDDDDDD"};

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
    //找到下标
    int i;
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
  HANDLE consumerSemEmpty = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, SEM_EMPTY);
  HANDLE consumerSemFull = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, SEM_FULL);
  HANDLE consumerMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, SEM_MUTEX);

  //生产者重复生产
  for (int i = 1; i <= PRODUCE_NUM; i++)
  {
    int start = time(NULL); //记录开始时间
    //随机一个等待时间
    Sleep((rand() % 2 + 1) * 1000);

    //两个P操作，先申请EMPTY，再申请MUTEX
    WaitForSingleObject(consumerSemEmpty, INFINITE);
    WaitForSingleObject(consumerMutex, INFINITE);

    //随机取一个字符串
    char *stock = stored_data[rand() % 4];
    //将字符串置入buffer尾部
    strcpy(SharedMemory->data.Buffer[SharedMemory->data.TAIL], stock);
    //将尾部指针向前移动一位
    SharedMemory->data.TAIL = (SharedMemory->data.TAIL + 1) % BUFFER_NUM;
    //将buffer为空置FALSE
    SharedMemory->data.ISEMPTY = FALSE;

    //记录结束时间
    int end = time(NULL);

    //输出本次操作内容
    cout << "producer" << i << " produces: " << stock << " cost time: " << end - start << endl;
    //输出buffer内容
    printBuffer(SharedMemory);

    //两个V操作，先释放MUTEX，再释放FULL
    ReleaseMutex(consumerMutex);
    ReleaseSemaphore(consumerSemFull, 1, NULL);
  }

  //解除进程对一个文件映射对象的映射
  UnmapViewOfFile(FilePointer);
  FilePointer = NULL;

  //关闭文件映射句柄
  CloseHandle(SharedFileHandle);

  return 0;
}
