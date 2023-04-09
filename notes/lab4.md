# 操作系统原理实验报告

## **实验名称:**   实验四 中断

**授课教师：**  张青

**学生姓名:**   黄世杰

**学生学号:**   21307038

### **1.** **实验要求**

完成4个Assignment：Assignment 1 复现Example1；Assignment 2 复现Example2，输出学号；Assignment 3更改Example默认的中断处理函数为自己编写的函数；Assignment 4仿照Example4实现跑马灯

### **2.** **实验过程**

#### **Assignment 1**

* C/C++代码调用汇编函数：汇编代码中需要global asm_func声明汇编函数为全局的，C代码中需要extern void asm_func()来声明，C++代码中需要extern "C" void asm_func()来声明；
* 汇编代码调用C/C++函数：汇编代码中需要extern cxx_function来声明，C++代码中需要extern "C" void cxx_function()来声明，这里的extern "C"是为了不让C++的编译器改变函数的名字
* 这里我的Makefile就是简单构建了各个.o文件，再生成最终的.out文件

#### Assignment 2

复现Example 2，将输出的Hello World改成学号

* 因为内核最开始会跳到entry.asm中的enter_kernel中，再跳转到setup_kernel中，在setup_kernel中又会跳转到汇编的asm_utils中执行asm_hello_world函数，在这个汇编函数里直接打印学号即可

#### Assignment 3

复现Example 3，更改默认中断处理函数为我编写的函数

* 我将中断向量号为0的除0错误的中断处理函数改成

#### Assignment 4

复现Example 4，实现一个跑马灯

* 我按照Example 4的直接输出时钟中断次数来修改，先用一个数组存储要输出的内容char s[21] = "21307038 Huangshijie"（总共有20个字符+最后终止字符'\\0'），对于时钟中断计数器times，每次取tmp = times % 40，若tmp < 20，就直接在对应位置输出对应字符，若tmp >= 20，就直接在对应位置删除字符，从而实现跑马灯（先从左到右输出所有字符，再从左到右删除所有字符，不断循环）

### **3.关键代码**

#### Assignment  1

function
![[Pasted image 20230409101041.png]]

Makefile
![[Pasted image 20230409101724.png]]

#### Assignment 2

asm_utils.asm
![[Pasted image 20230409102314.png]]

#### Assignment 3

asm_utils.asm
![[Pasted image 20230409102959.png]]

#### Assignment 4

interrupt.cpp
![[Pasted image 20230409103658.png]]

### **4.实验结果**

#### Assignment 1

![[f79f1f0d4e2424d5ecd65439fd84474.png]]

#### Assignment 2 

![[b161ecbf267bcce029c3c75a0014c90.png]]

#### Assignment 3 

默认的中断处理函数函数
![[e7b07eca5fa1bae488e4bc6d1c1e8a3.png]]

更改0号div中断处理函数
![[9c25e1c08e7961cc8d3dbd9edc82845.png]]

#### Assignment 4

跑马灯的截图（可以看上传的视频来看完整的跑马灯）
![[Pasted image 20230409103804.png]]

### 5.  **课后思考题**

1. C代码(.c)  经过**预处理** -> 预处理文件(.i)  经过**编译** -> 汇编文件(.s)  经过**汇编**-> 可重定位文件(.o)  经过**链接** -> 可执行文件(.out)
2. c_func.c经过**预处理**处理宏后生成与处理文件c_func.i（处理宏，如#include、#define、#ifndef）；再经过**编译**转换成汇编代码文件c_func.s（转成我们平常写的.asm文件）；再经过**汇编**转换成可重定位文件c_func.o（一个二进制文件，里面包含了ELF头、程序头表、section等信息）；最后经过**链接**生成可执行文件c_func.out（将多个.o文件合并在一起）
3. C和汇编是通过各自生成了.o可重定位文件之后，再一起经过链接从而结合起来生成可执行文件的。若需要结合起来，则C中要用到汇编的内容或者汇编中用到C的内容，且汇编中要global声明全局，C中要extern void asm_func()来声明来自汇编函数
4. 因为mbr.asm和bootloader.asm都是独立的一个汇编文件，可以直接生成可执行文件.bin，但entry.asm需要用到C++函数，需要和C++文件一起生成可执行文件，所以要先生成.obj可重定位文件再和C++生成的.o文件一起链接
5. 是
	* 关中断：中断发生时CPU会做一些硬件处理工作，会关中断
	* 保存断点：硬件处理，地址压栈
	* 识别中断源：根据中断类型识别中断源
	* 保存现场：FLAGS寄存器等压栈保存
	* 执行中断服务程序：通过IDTR找到IDT，通过中断向量号找到对应的中断描述符来找到对应的中断服务程序（如assignment-3的asm_unhandled_interrupt()）
	* 恢复现场并返回：恢复保存的内容，并iret返回（a3的asm_unhandled_interrupt并没有返回，而是用死循环代替）
6. 指针在内存中存的值就是地址，所以指针的本质就是地址
7. 中断门进入中断服务程序时CPU会自动将中断关闭，也就是将CPU的eflags寄存器的IF标志位符为，防止嵌套中断发生；陷阱门进入中断服务程序时维持IF标志位不变
8. 当发生时钟中断时，CPU会先根据IDTR找到IDT，并通过中断向量号找到对应的中断描述符来找到对应的中断处理程序（如时钟中断处理函数）
	* 首先跳转到asm_time_interrupt_handler()汇编函数保存上下文(pushad)，然后再跳转到c_time_interrupt_handler()的C函数打印对应的内容并将times计数值++，C函数返回后回到asm_interrupt+handler()汇编函数恢复上下文(popad)，同时发送EOI消息，否则下一次中断不会发生，最后iret返回

### **6. 总结**

本次lab中，我学会了C/C++与汇编的联合使用、中断的相关内容
1. C/C++和汇编互相使用函数时需要声明（汇编需要global asm_func，C/C++需要extern void asm_func()，C++需要声明函数为exter "C"），他们共同生成可执行文件时需要先生成.o文件再链接成可执行文件 
2. 中断发生时，首先通过IDTR找到IDT的位置（所以IDTR、IDT和需要我们初始化），再通过对应的中断向量号找到对应的中断描述符来找到对应的中断处理函数进行处理，之后iret返回
3. 硬件8259A的中断需要先初始化（向特定端口发送4个ICW），之后我们可以向8259A发送OCW字实现优先级、中断屏蔽字和EOI消息的动态改变
4. 中断处理程序的编写思路：
	1. 保护现场（push寄存器内容）
	2. 中断处理（主要处理程序）
	3. 恢复现场（iret，对于8259A芯片的中断需要在iret之前发送EOI消息）