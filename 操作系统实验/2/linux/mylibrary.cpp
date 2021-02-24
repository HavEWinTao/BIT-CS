#include "mylibrary.h"

using namespace std;

//P操作
void P(int semId, int semNum)
{
    sembuf sem_op;

    //申请一个资源，信号量减一
    sem_op.sem_num = semNum;
    sem_op.sem_op = -1;
    sem_op.sem_flg = 0;

    //信号量操作
    semop(semId, &sem_op, 1);
}

//V操作
void V(int semId, int semNum)
{
    sembuf sem_op;

    //释放一个资源，信号量加一
    sem_op.sem_num = semNum;
    sem_op.sem_op = 1;
    sem_op.sem_flg = 0;

    //信号量操作
    semop(semId, &sem_op, 1);
}

// 打印缓冲区中的内容
void printBuffer(sharedMemory *data)
{
    cout << "BUFFER: ";
    //如果缓冲区为空
    if (data->ISEMPTY == true)
    {
        cout << "EMPTY";
    }
    else
    {
        //找下标
        int i;
        if (data->TAIL - 1 >= data->HEAD)
        {
            i = data->TAIL - 1;
        }
        else
        {
            i = data->TAIL - 1 + BUFFER_NUM;
        }
        for (; i >= data->HEAD; i--)
        {
            cout << data->Buffer[i % BUFFER_NUM] << " ";
        }
    }
    cout << endl;
}

//生产者
void produce(int semId, int shmId, int producerId)
{
    //可能写入的数据
    char stored_data[][10] = {"AAAAAAAAA",
                              "BBBBBBBB",
                              "CCCCCCCCC",
                              "DDDDDDDDD"};

    int start = time(NULL); //记录开始时间
    srand(time(NULL));      //随机数种子
    sleep((rand() % 2 + 1));
    //获取一个随机字母作为货物
    char *Stock = stored_data[rand() % 4];

    //两次P操作，先EMPTY后MUTEX
    P(semId, EMPTY);
    P(semId, MUTEX);

    //获取共享内存区指针
    sharedMemory *addr;
    //将共享段附加到申请通信的进程地址空间
    addr = (sharedMemory *)shmat(shmId, 0, 0);

    //将字符串压入队尾
    strcpy(addr->Buffer[addr->TAIL], Stock);
    addr->TAIL = (addr->TAIL + 1) % BUFFER_NUM;
    addr->ISEMPTY = false;
    int end = time(NULL);

    //输出相关信息
    cout << "producer" << producerId << " produces: " << Stock << " cost time: " << end - start << endl;
    //打印共享缓冲区中的内容
    printBuffer(addr);

    shmdt(addr); //将共享段与进程之间解除连接

    //两次V操作，先MUTEX后FULL
    V(semId, MUTEX);
    V(semId, FULL);
}

void consume(int semId, int shmId, int consumerId)
{
    //两次P操作，先EMPTY后MUTEX
    P(semId, FULL);
    P(semId, MUTEX);

    int start = time(NULL); //记录开始时间
    srand(time(NULL));      //随机数种子
    //随机等待一个1~2秒的时间
    sleep((rand() % 2 + 1));

    //获取共享内存区指针
    sharedMemory *addr;
    //将共享段附加到申请通信的进程地址空间
    addr = (sharedMemory *)shmat(shmId, 0, 0);

    //将队列头的字符串取出
    char *Stock = addr->Buffer[addr->HEAD];
    addr->HEAD = (addr->HEAD + 1) % BUFFER_NUM;
    if (addr->HEAD == addr->TAIL)
    {
        addr->ISEMPTY = true;
    }

    int end = time(NULL); //记录结束时间

    //打印相关内容
    cout << "consumer" << consumerId << " consume: " << Stock << " cost time: " << end - start << endl;
    //打印共享缓冲区中的内容
    printBuffer(addr);

    shmdt(addr); //将共享段与进程之间解除连接

    //两次V操作，先MUTEX后FULL
    V(semId, MUTEX);
    V(semId, EMPTY);
}
