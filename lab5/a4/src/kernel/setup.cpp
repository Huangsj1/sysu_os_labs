#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 程序管理器
ProgramManager programManager;

void third_thread(void *arg) {
    int a = 5, p = 100000000, mod = 9999999;
    int tmp = 1;
    for (int i = 1; i <= p; i++) {
        tmp = (tmp * a) % mod;
        //if (i % 20000000 == 0) {
        //    printf("pid[%d]: (%d^%d)%%%d = %d\n", programManager.running->pid, a, i, mod, tmp);
        //}
    }
}
void second_thread(void *arg) {
    int a = 7, p = 100000000, mod = 9999999;
    int tmp = 1;
    for (int i = 1; i <= p; i++) {
        tmp = (tmp * a) % mod;
        //if (i % 20000000 == 0) {
        //    printf("pid[%d]: (%d^%d)%%%d = %d\n", programManager.running->pid, a, i, mod, tmp);
        //}
    }
}
void first_thread(void *arg)
{
    programManager.executeThread(second_thread, nullptr, "second thread");

    int a = 3, p = 100000000, mod = 9999999;
    int tmp = 1;
    for (int i = 1; i <= p; i++) {
        tmp = (tmp * a) % mod;
        if (i == (p / 2)) {
            programManager.executeThread(third_thread, nullptr, "third thread");
        }
        //if (i % 20000000 == 0) {
        //    printf("pid[%d]: (%d^%d)%%%d = %d\n", programManager.running->pid, a, i, mod, tmp);
        //}
    }
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
    int pid = programManager.executeThread(first_thread, nullptr, "first thread");
    if (pid == -1)
    {
        printf("can not execute thread\n");
        asm_halt();
    }

    int ReadyListIndex = programManager.notEmptyList();
    ListItem *item = programManager.readyPrograms[ReadyListIndex].front();
    PCB *firstThread = ListItem2PCB(item, tagInGeneralList);
    firstThread->status = RUNNING;
    programManager.readyPrograms[ReadyListIndex].pop_front();
    programManager.running = firstThread;
    asm_switch_thread(0, firstThread);

    asm_halt();
}
