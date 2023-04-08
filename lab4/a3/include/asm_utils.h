// C++中包含的头文件（用到汇编utils下的文件）
#ifndef ASM_UTILS_H
#define ASM_UTILS_H

#include "os_type.h"

extern "C" void asm_hello_world();
extern "C" void asm_unhandled_interrupt();
extern "C" void asm_lidt(uint32 start, uint16 limit);
extern "C" void asm_halt();

extern "C" void asm_div_handler();

#endif
