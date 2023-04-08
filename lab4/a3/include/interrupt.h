#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "os_type.h"

class InterruptManager
{
private:
    // IDT起始地址
    uint32 *IDT;

public:
    InterruptManager();

    // 初始化（包括IDT、所有的中断描述符）
    void initialize();

    // 设置中断描述副
    // index    第index个描述符（0，1，2...，255）
    // address  中断处理程序的起始地址
    // DPL      中断描述符的特权级
    void setInterruptDescriptor(uint32 index, uint32 address, byte DPL);
};

#endif
