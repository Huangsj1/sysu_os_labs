#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"

#define BUFFER_SIZE 5
#define PRODUCER_NUM 3
#define CONSUMER_NUM 3
#define ITERATE_TIME 3

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 程序管理器
ProgramManager programManager;

// 下面是有关生产者消费者的全局变量
int buf[BUFFER_SIZE];     // 缓冲区
int in = 0;               // 生产者放置
int out = 0;              // 消费者消费

Semaphore mutex;  // 访问临界区需要用的
Semaphore full;   // 有多少个资源可用
Semaphore empty;  // 有多少个位置是空的

void Producer(void *args) {
    for (int i = ITERATE_TIME; i > 0; --i) {
        // 进入临界区之前
        empty.P();
        mutex.P();
        // 进入临界区
        buf[in % BUFFER_SIZE] = i;
        // 模拟 in += 1 在汇编中的情况
        int tmp = in;
        tmp += 1;
        // 模拟下面的过长被调度
        programManager.schedule();
        //int delay = 0xfffffff;
        //while (delay) {
        //    --delay;
        //}
        in = tmp;
        // 退出临界区
        mutex.V();
        full.V();
    }
    printf("Producer finish, in: %d, out: %d\n", in, out);
}

void Consumer(void *args) {
    for (int i = ITERATE_TIME; i > 0; --i) {
        // 进入临界区之前
        full.P();
        mutex.P();
        // 进入临界区
        buf[out % BUFFER_SIZE] = 0;
        int tmp = out;
        tmp += 1;
        programManager.schedule();
        //int delay = 0xfffffff;
        //while (delay) {
        //    --delay;
        //}
        out = tmp;
        // 退出临界区
        mutex.V();
        empty.V();
    }
    printf("Consumer finish, in: %d, out: %d\n", in, out);
}

void first_thread(void *arg)
{
    // 第1个线程不可以返回
    stdio.moveCursor(0);
    for (int i = 0; i < 25 * 80; ++i)
    {
        stdio.print(' ');
    }
    stdio.moveCursor(0);

    // 初始化所有的信号量
    mutex.initialize(1);
    full.initialize(0);
    empty.initialize(BUFFER_SIZE);

    for (int i = 0; i < PRODUCER_NUM; ++i) {
        programManager.executeThread(Producer, nullptr, "producer", 1);
    }

    for (int i = 0; i < CONSUMER_NUM; ++i) {
        programManager.executeThread(Consumer, nullptr, "consumer", 1);
    }

    // sleep等待所有线程结束
    for (int i = 0; i < 2000000000; i++) {}

    printf("After all thread, in: %d, out: %d\n", in, out);

    asm_halt();
}

extern "C" void setup_kernel()
{

    // 中断管理器
    interruptManager.initialize();
    interruptManager.enableTimeInterrupt();
    interruptManager.setTimeInterrupt((void *)asm_time_interrupt_handler);

    // 输出管理器
    stdio.initialize();

    // 进程/线程管理器
    programManager.initialize();

    // 创建第一个线程
    int pid = programManager.executeThread(first_thread, nullptr, "first thread", 1);
    if (pid == -1)
    {
        printf("can not execute thread\n");
        asm_halt();
    }

    ListItem *item = programManager.readyPrograms.front();
    PCB *firstThread = ListItem2PCB(item, tagInGeneralList);
    firstThread->status = RUNNING;
    programManager.readyPrograms.pop_front();
    programManager.running = firstThread;
    asm_switch_thread(0, firstThread);

    asm_halt();
}
