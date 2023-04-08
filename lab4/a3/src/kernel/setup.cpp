#include "asm_utils.h"
#include "interrupt.h"

// 中断管理器
InterruptManager interruptManager;

extern "C" void setup_kernel()
{
    // 初始化中断管理器
    interruptManager.initialize();

    // 触发除0中断
    int a = 1 / 0;

    // 死循环
    asm_halt();
}
