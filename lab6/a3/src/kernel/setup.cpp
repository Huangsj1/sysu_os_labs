#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 程序管理器
ProgramManager programManager;

// 5根筷子的信号量资源
Semaphore chopstick[5];
Semaphore left;
Semaphore mutex;

void sleep() {
    int delay = 0xfffffff;
    while (delay > 0) {
        delay--;
    }
}

// A.3.1可能出现死锁的方案
void pl_thread_1(void *arg) {
    int i = *(int *)arg;
    //printf("[pid: %d] %d create\n", programManager.running->pid, i);
    while (1) {
        chopstick[i].P();
        chopstick[(i + 1) % 5].P();
        printf("[pid: %d] get chopstick %d & %d, is eating\n", programManager.running->pid, i, (i + 1) % 5);
        sleep();
        chopstick[i].V();
        chopstick[(i + 1) % 5].V();
        printf("[pid: %d] release chopstick %d & %d, finish eating\n", programManager.running->pid, i, (i + 1) % 5);
        sleep();
    }
}

// A.3.2.出现死锁
void pl_thread_2_0(void *arg) {
    int i = *(int *)arg;
    //printf("[pid: %d] %d create\n", programManager.running->pid, i);
    while (1) {
        chopstick[i].P();
        printf("[pid: %d] get chopstick %d\n", programManager.running->pid, i);
        sleep();
        chopstick[(i + 1) % 5].P();
        printf("[pid: %d] get chopstick %d & %d, is eating\n", programManager.running->pid, i, (i + 1) % 5);
        sleep();
        chopstick[i].V();
        chopstick[(i + 1) % 5].V();
        printf("[pid: %d] release chopstick %d & %d, finish eating\n", programManager.running->pid, i, (i + 1) % 5);
        sleep();
    }
}

// A.3.2(1)同时允许4个哲学家获得左边的筷子
void pl_thread_2_1(void *arg) {
    int i = *(int *)arg;
    //printf("[pid: %d] %d create\n", programManager.running->pid, i);
    while (1) {
        left.P();
        chopstick[i].P();
        printf("[pid: %d] get chopstick %d\n", programManager.running->pid, i);
        sleep();
        chopstick[(i + 1) % 5].P();
        printf("[pid: %d] get chopstick %d & %d, is eating\n", programManager.running->pid, i, (i + 1) % 5);
        sleep();
        sleep();
        chopstick[i].V();
        chopstick[(i + 1) % 5].V();
        printf("[pid: %d] release chopstick %d & %d, finish eating\n", programManager.running->pid, i, (i + 1) % 5);
        left.V();
        sleep();
    }
}

// A.3.2(2)哲学家获得筷子的时候要么同时获得左右的，要么不能获得
void pl_thread_2_2(void *arg) {
    int i = *(int *)arg;
    //printf("[pid: %d] %d create\n", programManager.running->pid, i);
    while (1) {
        mutex.P();
        chopstick[i].P();
        printf("[who: %d] get chopstick %d\n", i + 1, i);
        sleep();
        chopstick[(i + 1) % 5].P();
        printf("[who: %d] get chopstick %d & %d, is eating\n", i + 1, i, (i + 1) % 5);
        mutex.V();
        sleep();
        sleep();
        printf("[who: %d] release chopstick %d & %d, finish eating\n", i+1, i, (i + 1) % 5);
        chopstick[i].V();
        chopstick[(i + 1) % 5].V();
        sleep();
    }
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

    for (int i = 0; i < 5; ++i) {
        chopstick[i].initialize(1);
    }
    left.initialize(4);
    mutex.initialize(1);

    int index[5] = {0, 1, 2, 3, 4};
    // 前面3个的thread执行都可以用这个线程的创建
    for (int i = 0; i < 5; ++i) {
        programManager.executeThread(pl_thread_1, (void*)&index[i], "thread", 1);
    }
    //最后一个thread的创建如果按顺序就不能很好的演示
    //programManager.executeThread(pl_thread_2_2, (void*)&index[0], "thread", 1);
    //programManager.executeThread(pl_thread_2_2, (void*)&index[2], "thread", 1);
    //programManager.executeThread(pl_thread_2_2, (void*)&index[4], "thread", 1);
    //programManager.executeThread(pl_thread_2_2, (void*)&index[1], "thread", 1);
    //programManager.executeThread(pl_thread_2_2, (void*)&index[3], "thread", 1);


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
