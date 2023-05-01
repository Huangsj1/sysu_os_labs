#include "os_type.h"

template<typename T>
void swap(T &x, T &y) {
    T z = x;
    x = y;
    y = z;
}


void itos(char *numStr, uint32 num, uint32 mod) {
    // 只能转换2~36进制的整数
    if (mod < 2 || mod > 36 || num < 0) {
        return;
    }

    uint32 length, tmp;

    // 进制转化
    do {
        tmp = num % mod;
        num /= mod;
        numStr[length++] = tmp > 9 ? tmp - 10 + 'A' : tmp + '0';
    } while(num);

    // 将字符串倒转，使得numStr[0]保存的是num的高位数字
    for(int i = 0, j = length - 1; i < j; ++i, --j) {
        swap(numStr[i], numStr[j]);
    }

    numStr[length] = '\0';
}
