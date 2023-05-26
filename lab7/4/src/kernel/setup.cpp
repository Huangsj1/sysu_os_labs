#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"
#include "program.h"
#include "thread.h"
#include "sync.h"
#include "memory.h"

// 屏幕IO处理器
STDIO stdio;
// 中断管理器
InterruptManager interruptManager;
// 程序管理器
ProgramManager programManager;
// 内存管理器
MemoryManager memoryManager;

void first_thread(void *arg)
{
    // 第1个线程不可以返回
    // stdio.moveCursor(0);
    // for (int i = 0; i < 25 * 80; ++i)
    // {
    //     stdio.print(' ');
    // }
    // stdio.moveCursor(0);

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

    // 内存管理器
    memoryManager.openPageMechanism();
    memoryManager.initialize();

    //int start_1 = memoryManager.allocatePhysicalPages(KERNEL, 5);
    //int start_2 = memoryManager.allocatePhysicalPages(KERNEL, 5);
    //int start_3 = memoryManager.allocatePhysicalPages(KERNEL, 5);
    //int start_4 = memoryManager.allocatePhysicalPages(KERNEL, 3);
    //int start_5 = memoryManager.allocatePhysicalPages(KERNEL, 5);
    //memoryManager.releasePhysicalPages(KERNEL, start_2, 5);
    //memoryManager.releasePhysicalPages(KERNEL, start_4, 3);
    //int start_new = memoryManager.allocatePhysicalPages(KERNEL, 3);

    int all_length = memoryManager.kernelPhysical.resources.length;
    printf("kernel max physical page length: %d\n", all_length);
    int start_1 = memoryManager.allocatePhysicalPages(KERNEL, all_length / 2);
    int start_2 = memoryManager.allocatePhysicalPages(KERNEL, all_length / 2);
    int start_out = memoryManager.allocatePhysicalPages(KERNEL, all_length / 2);

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
