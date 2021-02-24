#include <sys/wait.h>

#include "mylibrary.h"

using namespace std;

int main(int argc, char *argv[])
{
    //主进程开始
    cout << "main process start" << endl;
    
    //共享内存区
    sharedMemory *addr;
    //共享内存区控制块进行初始化
    int shmid = shmget(IPC_PRIVATE, BUFFER_SIZE, MODE);
    //初始化共享内存区
    addr = (sharedMemory *)shmat(shmid, 0, 0);
    addr->HEAD = 0;
    addr->TAIL = 0;
    addr->ISEMPTY = true;

    union semun semUnion;
    //创建信号量集合，创建两个同步信号量和一个互斥信号量
    int semId = semget(IPC_PRIVATE, 3, MODE);
    //互斥信号量MUTEX
    semUnion.val = 1;
    semctl(semId, MUTEX, SETVAL, semUnion);
    //同步信号量FULL
    semUnion.val = 0;
    semctl(semId, FULL, SETVAL, semUnion);
    //同步信号量EMPTY
    semUnion.val = BUFFER_SIZE;
    semctl(semId, EMPTY, SETVAL, semUnion);

    //生产者
    for (int i = 1; i <= PRODUCER_PROCESS_NUM; i++)
    {
        pid_t producerProcess = fork();
        //创建失败
        if (producerProcess < 0)
        {
            cerr << "created fail!" << endl;
            exit(1);
        }
        //位于子进程
        if (producerProcess == 0)
        {
            cout << "producer" << i << endl;
            //重复生产
            for (int j = 1; j <= PRODUCE_NUM; j++)
            {
                produce(semId, shmid, j);
            }
            exit(0);
        }
    }

    //消费者
    for (int i = 1; i <= CONSUMER_PROCESS_NUM; i++)
    {
        pid_t consumerProcess = fork();
        //创建失败
        if (consumerProcess < 0)
        {
            cerr << "create faile!" << endl;
            exit(1);
        }
        //位于子进程
        if (consumerProcess == 0)
        {
            cout << "consumer" << i << endl;
            //重复消费
            for (int j = 1; j <= CONSUME_NUM; j++)
            {
                consume(semId, shmid, j);
            }
            exit(0);
        }
    }

    //父进程等待两个子进程返回之后继续执行
    while (wait(0) > 0)
    {
        continue;
    }

    //断开链接的共享内存的指针
    shmdt(addr);

    //运行结束
    cout << "main process end" << endl;

    return 0;
}
