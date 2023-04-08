#include "asm_utils.h"
#include "interrupt.h"
#include "stdio.h"

// 中断管理器
InterruptManager interruptManager;
// 屏幕IO处理器
STDIO stdio;

extern "C" void setup_kernel()
{
    // 初始化中断管理器
    interruptManager.initialize();
    // 初始化IO处理器
    stdio.initialize();
    //设置8259A芯片可用时钟中断
    interruptManager.enableTimeInterrupt();
    interruptManager.setTimeInterrupt((void *)asm_time_interrupt_handler);
    //开中断
    asm_enable_interrupt();
    // 死循环
    asm_halt();
}
