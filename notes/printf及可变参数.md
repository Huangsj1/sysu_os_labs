## 可变参数的使用

| 宏                                  | 用法说明                                                                                   |
| ----------------------------------- | ------------------------------------------------------------------------------------------ |
| va_list                             | 定义一个指向可变参数列表的**指针**                                                             |
| void va_start(va_list ap, last_arg) | **初始化**可变参数列表指针，使其指向可变参数列表*第一个位置*，即固定参数最后一个参数last_arg之后 |
| type va_arg(va_list ap, type)       | type是一个类型，以该类型**返回可变参数**，同时**ap指向下一个参数**                                 |
| void va_end(va_list ap)             | 清除ap(清零)                                                                                     |       

## 可变参数的实现

```c
typedef char * va_list
#define _INTSIZEOF(n) ( (sizeof(n)+sizeof(int)-1) & ~(sizeof(int)-1))
#define va_start(ap, v) ( ap = (va_list)&v + _INTSIZEOF(v) )
#define va_arg(ap, type) ( *(type *)((ap += _INTSIZEOF(type)) - _INTSIZEOF(type)))
#define va_end(ap) ( ap = (va_list)0)
```

* `va_list` 是单字节类型的指针，即**char \***，经过初始化 `va_start()` 后他指向保存参数的**栈中的可变参数的第一个位置**
* `_INTSIZEOF(n)` 是根据n的类型进行**四字节对齐**（(n + 4 - 1) / 4 * 4）
* 由于在保护模式下栈的push和pop都是32位，所以无论什么类型在栈中保存的都是4个字节的倍数，所以上面在取值的时候要按4个字节的整数倍取

```c
// 访问可变参数(这里第一个参数为可变参数数目,但其实是什么都可以)
void print_changable_para(int n, ...) {
	// 1.定义指针
	va_list parameter;
	// 2.初始化指针
	va_start(parameter, n);
	// 遍历获得参数
	for (int i = 0; i < n; i++) {
		// 3.获取参数
		std::cout << va_arg(parameter, int) << " ";
	}
	// 4.清零
	va_end(parameter);
}
```

## printf () 的实现

```c
// printf的可变参数
int printf(const char* const _Format, ...);
```

* 通过一个缓存区，不断读取_Format中的字符进去：
	* **普通字符**就直接放进去
	* 转义字符( \n, \t, \0等)就特殊判断
	* %字符就判断后面的字符(%d / %c / %s / %x)，从可变参数中读取并输出

```c
int printf(const char *const fmt, ...) {
	const int BUF_SIZE = 32;
	
	char buffer[BUF_SIZE + 1];    // 缓冲区
	char number[33]               // 存储可变参数的正数类型的字符串

	int idx, counter;
	va_list ap;              // 1.指针定义

	va_start(ap, fmt);       // 2.指针初始化
	idx = 0;             // buffer的下标
	counter = 0;         // 打印的字符个数

	for (int i = 0; fmt[i]; ++i) {
		// 如果是普通字符
		if (fmt[i] != '%') {
			// 将字符加到buffer中，如果超出了buffer的大小就输出并清空
			counter += add_to_buffer(buffer, fmt[i], idx, BUF_SIZE);
		} else{
			i++;
			if (fmt[i] == '\0') {
				break;
			}
			switch (fmt[i]) {
			case '%':
				counter += add_to_buffer(buffer, fmt[i], idx, BUF_SIZE);
				break;
			// 直接输出字符的ASCII码
			case 'c':
				counter += add_to_buffer(buffer, va_arg(ap, int), idx, BUF_SIZE);
				break;
			// 直接输出缓冲区 且 输出对应字符串
			case 's':
				buffer[idx] = '\0';
				idx = 0;
				counter += stdio.print(buffer);
				counter += stdio.print(va_arg(ap, const char *));
				break;
			// 数字就以对应的格式转成字符串再写入缓冲区
			case 'd':
			case 'x':
				// 获取对应的数字
				int tmp = va_arg(ap, int);
				if(tmp < 0 && fmt[i] == 'd') {
					counter += add_to_buffer(buffer, '-', idx, BUF_SIZE);
					tmp = -tmp;
				}
				// 这里将数字转成字符串(逆序)写入buffer，同时返回的是长度
				tmp = itos(number, tmp, (fmt[i] == 'd' ? 10 : 16));
				for (int j = 0; j < tmp - 1; j++) {
					counter += add_to_buffer(buffer, number[j], idx, BUF_SIZE);
				}
				break;
			}
		}
	}
	// 输出缓冲区所有内容
	buffer[idx] = '\0';
	counter += stdio.print(buffer);
	// 4.清零指针
	va_end(ap);

	return counter;
}


// 向缓冲区写入字符，若超出大小就输出
int add_to_buffer(char *buffer, char c, int &idx, const int BUF_SIZE) {
	int counter = 0;

	buffer[idx++] = c;
	if (idx == BUF_SIZE) {
		buffer[idx] = '\0';
		counter = stdio.print(buffer);
		idx = 0;
	}
	return counter;
}

// itos将数字转化为对应进制的字符串并写入numStr中
void itos(char *numStr, uint32 num, uint32 mod) {
	// 只能转换2~36进制的正数，因为字母只有26个(26+10)
	if (mod < 2 || mod > 36 || num < 0) {
		return;
	}
	
	uint 32 length, tmp;
	// 进制转化
	length = 0;
	do {
		tmp = num % mod;
		num /= mod;
		numStr[length++] = tmp > 9 ? tmp - 10 + 'A' : tmp + '0';
	} while(num);
	// 字符串倒转
	for (int i = 0, j = length - 1; i < j; i++, j--) {
		swap(numStr[i], numStr[j]);
	}
	numStr[length] = '\0';
}

// 交换模板
template<typename T>
void swap(T &x, T &y) {
	T z = x;
	x = y;
	y = z;
}
```
