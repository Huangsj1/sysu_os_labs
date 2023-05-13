#include "sync.h"
#include "asm_utils.h"
#include "stdio.h"
#include "os_modules.h"
#include "program.h"

SpinLock::SpinLock()
{
    initialize();
}

void SpinLock::initialize()
{
    bolt = 0;
}

void SpinLock::lock()
{
    //uint32 key = 1;

    //printf("[pid: %d] want to access\n", programManager.running->pid);

    while (asm_atomic_bts(&bolt) == 1) {}

    //printf("[pid: %d] accesses\n", programManager.running->pid);

    //do
    //{
        //asm_atomic_exchange(&key, &bolt);
        //printf("pid: %d\n", programManager.running->pid);
    //} while (key);
}

void SpinLock::unlock()
{
    //printf("[pid: %d] releases\n", programManager.running->pid);

    bolt = 0;
}

Semaphore::Semaphore()
{
    initialize(0);
}

void Semaphore::initialize(uint32 counter)
{
    this->counter = counter;
    semLock.initialize();
    waiting.initialize();
}

void Semaphore::P()
{
    PCB *cur = nullptr;

    //printf("[pid: %d] want to access\n", programManager.running->pid);

    while (true)
    {
        // 获得锁访问counter
        semLock.lock();
        // 如果counter>0表示有资源可用
        if (counter > 0)
        {
            --counter;
            semLock.unlock();

            //printf("[pid: %d] accesses\n", programManager.running->pid);

            return;
        }
        // 否则将当前线程加入到阻塞队列，并将状态设为BLOCKED
        cur = programManager.running;
        waiting.push_back(&(cur->tagInGeneralList));
        cur->status = ProgramStatus::BLOCKED;

        semLock.unlock();

        //printf("[pid: %d] is blocked\n", programManager.running->pid);

        programManager.schedule();
    }
}

void Semaphore::V()
{
    semLock.lock();
    // 释放资源
    ++counter;

    //printf("[pid: %d] leaves\n", programManager.running->pid);

    if (waiting.size())
    {
        PCB *program = ListItem2PCB(waiting.front(), tagInGeneralList);
        waiting.pop_front();
        semLock.unlock();
        // 唤醒阻塞线程
        programManager.MESA_WakeUp(program);
    }
    else
    {
        semLock.unlock();
    }
}
