#include <tchar.h>

//共享主存大小512字节
const int SHARED_MEM_SIZE = 512;

//缓冲区个数为6
const int BUFFER_NUM = 6;

const int BUFFER_SIZE = 10;

//生产者进程2个
const int PRODUCER_PROCESS_NUM = 2;
//消费者进程3个
const int CONSUMER_PROCESS_NUM = 3;
//进程总数5
const int TOTAL_PROCESS_COUNT = 5;

//2个生产者重复12次，3个消费者重复8次
const int PRODUCE_NUM = 12;
const int CONSUME_NUM = 8;

//生产者、消费者可执行文件
TCHAR PRODUCER_PATH[] = TEXT("producer.exe");
TCHAR CONSUMER_PATH[] = TEXT("consumer.exe");

//共享缓冲文件名
TCHAR SHM_NAME[] = TEXT("BUFFER");

//三个信号量名
TCHAR SEM_EMPTY[] = TEXT("EMPTY");
TCHAR SEM_FULL[] = TEXT("FULL");
TCHAR SEM_MUTEX[] = TEXT("MUTEX");

//缓冲区
struct BUFFER
{
  char Buffer[BUFFER_NUM][BUFFER_SIZE];
  int HEAD;     //队列头
  int TAIL;     //队列尾
  bool ISEMPTY; //判断是否为空
};

//共享主存区
struct sharedMemory
{
  BUFFER data; //缓冲区
  //信号量
  HANDLE Full;
  HANDLE Empty;
  HANDLE Mutex;
};
