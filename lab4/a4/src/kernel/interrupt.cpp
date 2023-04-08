#include "interrupt.h"
#include "os_type.h"
#include "os_constant.h"
#include "asm_utils.h"
#include "stdio.h"

extern STDIO stdio;

int times = 0;      // 时钟中断次数

InterruptManager::InterruptManager()
{
    initialize();
}

void InterruptManager::initialize()
{
    IDT = (uint32 *)IDT_START_ADDRESS;
    asm_lidt(IDT_START_ADDRESS, 256 * 8 - 1);

    for(uint i = 0; i < 256; i++) {
        setInterruptDescriptor(i, (uint32)asm_unhandled_interrupt, 0);
    }

    // 设置div中断
    setInterruptDescriptor(0, (uint32)asm_div_handler, 0);

    times = 0;
    initialize8259A();    // 初始化8259A芯片
}

void InterruptManager::setInterruptDescriptor(uint32 index, uint32 address, byte DPL)
{
    //中断描述符的低32位
    IDT[index * 2] = (CODE_SELECTOR << 16) | (address & 0xffff);
    //中断描述符的高32位
    IDT[index * 2 + 1] = (address & 0xffff0000) | (0x1 << 15) | (DPL << 13) | (0xe << 8);
}

void InterruptManager::initialize8259A()
{
    // ICW 1
    asm_out_port(0x20, 0x11);
    asm_out_port(0xa0, 0x11);
    // ICW 2
    IRQ0_8259A_MASTER = 0x20;
    IRQ0_8259A_SLAVE = 0x28;
    asm_out_port(0x21, IRQ0_8259A_MASTER);
    asm_out_port(0xa1, IRQ0_8259A_SLAVE);
    // ICW 3
    asm_out_port(0x21, 4);
    asm_out_port(0xa1, 2);
    // ICW 4
    asm_out_port(0x21, 1);
    asm_out_port(0xa1, 1);

    // OCW 1 屏蔽主片所有中断，但主片的IRQ2需要开启
    asm_out_port(0x21, 0xfb);
    // OCW 1 屏蔽从片所有中断
    asm_out_port(0xa1, 0xff);
}

void InterruptManager::enableTimeInterrupt()
{
    uint8 value;
    // 读取主片OCW
    asm_in_port(0x21, &value);
    // 开启主片时钟中断, 置0开启
    value = value & 0xfe;
    asm_out_port(0x21, value);
}

void InterruptManager::disableTimeInterrupt()
{
    uint8 value;
    asm_in_port(0x21, &value);
    // 关闭时钟中断，置1关闭
    value = value | 0x01;
    asm_out_port(0x21, value);
}

void InterruptManager::setTimeInterrupt(void *handler)
{
    setInterruptDescriptor(IRQ0_8259A_MASTER, (uint32)handler, 0);
}

// 时钟中断跑马灯
extern "C" void c_time_interrupt_handler()
{
    // 总共有20个要显示的字符
    char light[21] = "21307038 Huangshijie";
    int pos;

    // 初始时先清空第一行
    if(times == 0) {
        for(int i = 0; i < 80; i++) {
            stdio.print(0, i, ' ', 0x07);
        }
    }

    pos = times % 40;
    ++times;
    // 前20为显示对应位置的字符, 后20表示清除对应位置的字符
    if(pos < 20) {
        stdio.print(0, pos, light[pos], 0x03);
    } else {
        stdio.print(0, pos - 20, ' ', 0x07);
    }

}

// 时钟中断的处理函数
//extern "C" void c_time_interrupt_handler()
//{
//    // 清空屏幕的第一行
//    for(int i = 0; i < 80; i++) {
//        stdio.print(0, i, ' ', 0x07);
//    }
//
//    // 输出时钟中断发生的次数
//    ++times;
//    char str[] = "interrupt happend: ";
//    char number[10];
//    int tmp = times;
//
//    // 将数字转化为字符串
//    for(int i = 0; i < 10; i++) {
//        if(tmp) {
//            number[i] = tmp % 10 + '0';
//        } else {
//            number[i] = '0';
//        }
//        tmp /= 10;
//    }
//
//    // 移动光标到(0, 0)输出字符
//    stdio.moveCursor(0);
//    for(int i = 0; str[i]; i++) {
//        stdio.print(str[i]);
//    }
//
//    // 输出中断发生次数
//    for(int i = 9; i > 0; i--) {
//        stdio.print(number[i]);
//    }
//}
