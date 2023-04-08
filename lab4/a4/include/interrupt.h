#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "os_type.h"

class InterruptManager
{
private:
    uint32 *IDT;                // IDT起始地址
    uint32 IRQ0_8259A_MASTER;   // 主片中断起始向量号
    uint32 IRQ0_8259A_SLAVE;    // 从片中断起始向量号

public:
    InterruptManager();
    // 初始化（包括IDT、所有的中断描述符）
    void initialize();
    // 设置中断描述副
    // index    第index个描述符（0，1，2...，255）
    // address  中断处理程序的起始地址
    // DPL      中断描述符的特权级
    void setInterruptDescriptor(uint32 index, uint32 address, byte DPL);

    void enableTimeInterrupt();         // 开启时钟中断
    void disableTimeInterrupt();        // 关闭时钟中断
    void setTimeInterrupt(void *handler);   // 设置时钟中断处理函数

private:
    void initialize8259A();     //初始化8259A芯片
};

#endif
