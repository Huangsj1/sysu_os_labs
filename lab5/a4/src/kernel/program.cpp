#include "program.h"
#include "stdlib.h"
#include "interrupt.h"
#include "asm_utils.h"
#include "stdio.h"
#include "thread.h"
#include "os_modules.h"

const int PCB_SIZE = 4096;                   // PCB的大小，4KB。
char PCB_SET[PCB_SIZE * MAX_PROGRAM_AMOUNT]; // 存放PCB的数组，预留了MAX_PROGRAM_AMOUNT个PCB的大小空间。
bool PCB_SET_STATUS[MAX_PROGRAM_AMOUNT];     // PCB的分配状态，true表示已经分配，false表示未分配。

ProgramManager::ProgramManager()
{
    initialize();
}

void ProgramManager::initialize()
{
    allPrograms.initialize();
    // 初始化所有(3个)调度队列；第一个队列优先级为1，往下每个+1
    for (int i = 0; i < QUEUE_NUM; ++i) {
        readyPrograms[i].initialize();
        quePriority[i] = i + 1;
    }
    running = nullptr;

    for (int i = 0; i < MAX_PROGRAM_AMOUNT; ++i)
    {
        PCB_SET_STATUS[i] = false;
    }
}

int ProgramManager::executeThread(ThreadFunction function, void *parameter, const char *name)
{
    // 关中断，防止创建线程的过程被打断
    bool status = interruptManager.getInterruptStatus();
    interruptManager.disableInterrupt();

    // 分配一页作为PCB
    PCB *thread = allocatePCB();

    if (!thread)
        return -1;

    // 初始化分配的页
    memset(thread, 0, PCB_SIZE);

    for (int i = 0; i < MAX_PROGRAM_NAME && name[i]; ++i)
    {
        thread->name[i] = name[i];
    }

    // 所有线程进入的时候优先级都最高，为1
    int priority = 1;
    thread->status = ProgramStatus::READY;
    thread->priority = priority;
    thread->ticks = priority * 10;
    thread->ticksPassedBy = 0;
    thread->pid = ((int)thread - (int)PCB_SET) / PCB_SIZE;

    // 线程栈
    thread->stack = (int *)((int)thread + PCB_SIZE);
    thread->stack -= 7;
    thread->stack[0] = 0;
    thread->stack[1] = 0;
    thread->stack[2] = 0;
    thread->stack[3] = 0;
    thread->stack[4] = (int)function;
    thread->stack[5] = (int)program_exit;
    thread->stack[6] = (int)parameter;

    allPrograms.push_back(&(thread->tagInAllList));
    readyPrograms[0].push_back(&(thread->tagInGeneralList));

    // 恢复中断
    interruptManager.setInterruptStatus(status);

    printf("pid:%d was added into waiting queue\n", thread->pid);

    // 抢占式调度
    if (running->priority > priority) {
        schedule();
    }

    return thread->pid;
}

void ProgramManager::schedule()
{
    bool status = interruptManager.getInterruptStatus();
    interruptManager.disableInterrupt();

    int ReadyListIndex = notEmptyList();
    // 如果所有队列都没有PCB
    if (ReadyListIndex == -1)
    {
        interruptManager.setInterruptStatus(status);
        return;
    }

    printf("pid:%d stop(left %d ticks)\n", running->pid, running->ticks);

    if (running->status == ProgramStatus::RUNNING)
    {
        running->status = ProgramStatus::READY;
        // 如果时间片用完了且还未到达最低优先级降低优先级
        if (running->ticks == 0 && running->priority < 3) {
            running->priority += 1;
        }
        running->ticks = running->priority * 10;
        // 根据优先级放到对应的队列中
        readyPrograms[running->priority - 1].push_back(&(running->tagInGeneralList));
    }
    else if (running->status == ProgramStatus::DEAD)
    {
        releasePCB(running);
    }

    ListItem *item = readyPrograms[ReadyListIndex].front();
    PCB *next = ListItem2PCB(item, tagInGeneralList);
    PCB *cur = running;
    next->status = ProgramStatus::RUNNING;
    running = next;
    readyPrograms[ReadyListIndex].pop_front();

    printf("-----------------------------------------\n");
    printf("pid:%d start(have %d ticks)\n", next->pid, next->ticks);

    asm_switch_thread(cur, next);

    interruptManager.setInterruptStatus(status);
}

void program_exit()
{
    PCB *thread = programManager.running;
    thread->status = ProgramStatus::DEAD;

    printf("exit: pid:%d\n", thread->pid);

    if (thread->pid)
    {
        programManager.schedule();
    }
    else
    {
        interruptManager.disableInterrupt();
        printf("halt\n");
        asm_halt();
    }
}

PCB *ProgramManager::allocatePCB()
{
    for (int i = 0; i < MAX_PROGRAM_AMOUNT; ++i)
    {
        if (!PCB_SET_STATUS[i])
        {
            PCB_SET_STATUS[i] = true;
            return (PCB *)((int)PCB_SET + PCB_SIZE * i);
        }
    }

    return nullptr;
}

void ProgramManager::releasePCB(PCB *program)
{
    int index = ((int)program - (int)PCB_SET) / PCB_SIZE;
    PCB_SET_STATUS[index] = false;
}


// 返回多级队列的第一个非空队列下标，没有就返回-1
int ProgramManager::notEmptyList() {
    for (int i = 0; i < QUEUE_NUM; i++) {
        if (readyPrograms[i].size() != 0) {
            return i;
        }
    }
    return -1;
}
