#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

//P、V操作
void P(int semId, int semNum);
void V(int semId, int semNum);

//消费者生产者进程
void produce(int semId, int shmId, int producerId);
void consume(int semId, int shmId, int consumerId);

//读写权限权限：读写
#define MODE 0600

//共享主存大小512字节
const int SHARED_MEM_SIZE = 512;

//缓冲区个数为 6
const int BUFFER_NUM = 6;
//缓冲区大小为 10
const int BUFFER_SIZE = 10;

//生产者进程2个
const int PRODUCER_PROCESS_NUM = 2;
//消费者进程3个
const int CONSUMER_PROCESS_NUM = 3;
//2 个生产者重复 12 次，3 个消费者重复 8 次
const int PRODUCE_NUM = 12;
const int CONSUME_NUM = 8;

union semun
{
  int val;
  struct semid_ds *buf;
  unsigned short *array;
};

//信号集的索引标志
const int MUTEX = 0;
const int FULL = 1;
const int EMPTY = 2;

//共享主存区
struct sharedMemory
{
  char Buffer[BUFFER_NUM][BUFFER_SIZE]; //缓冲区
  int HEAD;                             //队列头
  int TAIL;                             //队列尾
  bool ISEMPTY;                         //判断是否为空
};
