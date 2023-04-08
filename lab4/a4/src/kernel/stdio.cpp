#include "stdio.h"
#include "os_type.h"
#include "asm_utils.h"

STDIO::STDIO()
{
    initialize();
}

void STDIO::initialize()
{
    screen = (uint8 *)0xb8000;
}

void STDIO::print(uint x, uint y, uint8 c, uint8 color)
{
    if(x < 0 || x >= 25 || y < 0 || y >= 80) {
        return;
    }

    uint pos = x * 80 + y;
    screen[2 * pos] = c;
    screen[2 * pos + 1] = color;
}

void STDIO::print(uint8 c, uint8 color)
{
    uint cursor = getCursor();
    screen[2 * cursor] = c;
    screen[2 * cursor + 1] = color;
    cursor++;
    if(cursor == 25 * 80) {
        rollUp();
        cursor = 24 * 80;
    }
    moveCursor(cursor);
}

void STDIO::print(uint8 c)
{
    print(c, 0x07);
}

// 参数是将位置坐标转化为一位的
void STDIO::moveCursor(uint position)
{
    if(position >= 80 * 25) {
        return;
    }

    uint tmp;

    // 处理高8位
    tmp = (position >> 8) & 0xff;
    asm_out_port(0x3d4, 0x0e);
    asm_out_port(0x3d5, tmp);

    // 处理低8位
    tmp = position & 0xff;
    asm_out_port(0x3d4, 0x0f);
    asm_out_port(0x3d5, tmp);
}

uint STDIO::getCursor()
{
    uint pos;
    uint8 tmp;

    pos = 0;
    tmp = 0;

    // 处理高8位
    asm_out_port(0x3d4, 0x0e);
    asm_in_port(0x3d5, &tmp);
    pos = ((uint)tmp) << 8;

    // 处理低8位
    asm_out_port(0x3d4, 0x0f);
    asm_in_port(0x3d5, &tmp);
    pos = pos | ((uint)tmp);

    return pos;
}

void STDIO::moveCursor(uint x, uint y)
{
    if(x < 0 || x >= 25 || y < 0 || y >= 80) {
        return;
    }

    moveCursor(x * 80 + y);
}

void STDIO::rollUp()
{
    uint length;
    length = 25 * 80;
    // 从第二行开始，将当前行的内容上移
    for(uint i = 80; i < length; i++) {
        screen[2 * (i - 80)] = screen[2 * i];
        screen[2 * (i - 80) + 1] = screen[2 * i + 1];
    }

    // 将最后一行清空
    for(uint i = 24 * 80; i < length; i++) {
        screen[2 * i] = ' ';
        screen[2 * i + 1] = 0x07;
    }
}
