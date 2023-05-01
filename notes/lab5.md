# 操作系统原理实验报告

## **实验名称:**   实验五 内核线程

**授课教师：**  张青

**学生姓名:**   黄世杰

**学生学号:**   21307038

### **1.** **实验要求**

完成4个Assignment：Assignment 1: printf的实现；Assignment 2: 线程的实现；Assignment 3：线程调度切换的秘密；Assignment 4：调度算法的实现

### **2.** **实验过程**

#### **Assignment 1**

根据实验指导手册和源代码学习可变参数机制和实现printf。我修改了进制转化为36进制，且用 `do { } while ( ) ;` 减少特判0的情况；同时 `kernel/stdio.cpp` 中的 `printf()` 函数对于整数转成字符串后写入到buffer中和实验指导上的不一样，这里是直接从字符串numStr的开始写到结尾，而指导上的是从length-1写到0，指导上顺序错了（因为经过 `itos()` 转化后数字高位写到了numStr低位上）

#### Assignment 2

因为实验代码中设计的PCB已经较为完善，所需要的各种功能都完备，而且对于后面调度算法所需要的优先级、时间片等都有包含，所以我就直接用代码中给的PCB来完成后面的任务

#### Assignment 3

通过gdb对线程创建、线程调度切换的整个过程进行跟踪，观察线程切换前后栈、寄存器等的变化。

线程的切换修改PCB的内容和ProgramManager里running的PCB，然后再切换栈

`asm_switch_thread()` 只是保存、切换、恢复栈而已

```c
asm_switch_thread:
    push ebp
    push ebx
    push edi
    push esi
    
    mov eax, [esp + 5 * 4]
    mov [eax], esp ; 保存当前栈指针到PCB中，以便日后恢复
    
    mov eax, [esp + 6 * 4]
    mov esp, [eax] ; 此时栈已经从cur栈切换到next栈
    
    pop esi
    pop edi
    pop ebx
    pop ebp
    sti
    ret
```

在 `schedule()` 中会调用 `asm_switch_thread(cur, next)` ，调用之前会先将 next 和 cur 压栈，再 `call` ，这里也会将ra即 `asm_switch_thread()` 之后的地址压栈；

进入 `asm_switch_thread()` 后又会 push 4个寄存器，此时\[esp + 5 * 4] 就是第一个参数 cur 的地址，因为PCB::stack为第一个成员，所以cur的地址也是stack的地址；而\[esp + 6 * 4] 是第二个参数next的地址，所以next的地址也是对应的stack的地址


#### Assignment 4

修改原本的轮转算法，该进成基于多级反馈队列调度的抢占式的算法

1. `include/program.h` 的ProgramManager结构的定义的修改
	1. 原本一个调度队列修改为3级反馈队列，优先级从1~3逐渐下降(值越小优先级越高)，
	2. 同时用一个数组记录每个队列的优先级，将来为队列中的PCB分配的时间片为优先级\*10（优先级越高，分配的时间片应该越短，才能让高优先级的队列较快执行并切换）
	3. 添加相关函数 `notEmpryList()` 返回第一个非空的队列的下标
2. 修改 `initialize()` 初始化：改为所有 (3个) readyPrograms的初始化；同时初始化所有调度队列的优先级: 第一个队列优先级为1 (最高)，往下每个队列优先级+1（我这里priority的值越低, 优先级越高）
3. 在 `executeThread()`  中减少最后一个参数priority，将所有新进来的线程的优先级设置为1，并放到调度队列的第一个队列中
4. 在 `schedule()` 中从多级队列中调度PCB
5. 添加函数 `notEmptyList()` 返回多级反馈队列中第一个有空闲PCB的队列的下标，供调度时使用
6. 当线程退出时打印exit
7. 修改3个不同的thread执行函数，其中第三个线程会在第一个线程执行时抢占第一个，使得第一个线程的时间片还未用完就切换

### **3.关键代码

#### Assignment 1

`printf()` 实现
![[Pasted image 20230501110405.png]]

`itos()` 修改
![[Pasted image 20230428152204.png]]

#### Assignment 2

PCB结构定义
```c
struct PCB
{
    int *stack;                      // 栈指针，用于调度时保存esp
    char name[MAX_PROGRAM_NAME + 1]; // 线程名
    enum ProgramStatus status;       // 线程的状态
    int priority;                    // 线程优先级
    int pid;                         // 线程pid
    int ticks;                       // 线程时间片总时间
    int ticksPassedBy;               // 线程已执行时间
    ListItem tagInGeneralList;       // 线程队列标识
    ListItem tagInAllList;           // 线程队列标识
};
```


#### Assignment 3

1. 初始线程在 `setup_kernel()` 中的初始化各种管理器后被创建 `executeThread()`，然后通过 `asm_switch_thread()` 来进行调度，返回到 `first_thread()` 函数中执行

![[Pasted image 20230428203823.png]]

![[Pasted image 20230428203835.png]]


当时间片到的时候会调用 `c_timer_interrupt()` 来调用 `schedule()` 调度然后再掉用 `asm_switch_thread(cur, next)` 切换线程

![[Pasted image 20230429091423.png]]


#### Assignment 4

1. 修改 `ProgramManager` 结构 
![[Pasted image 20230429110803.png]]

2. 修改 `initialize()` 初始化函数
![[Pasted image 20230429101604.png]]

3. 修改 `executeThread()` 将新线程加入到调度队列
![[Pasted image 20230501111905.png]]

4. 在 `schedule()` 中从多级队列中调度PCB
![[Pasted image 20230501112050.png]]

5. 添加函数 `notEmptyList()` 返回多级反馈队列中第一个有空闲PCB的队列的下标
![[Pasted image 20230501112224.png]]

6. 当线程退出时打印exit
![[Pasted image 20230501112158.png]]

7. 修改3个不同的thread执行函数，其中第三个线程会在第一个线程执行时抢占第一个，使得第一个线程的时间片还未用完就切换
![[Pasted image 20230501112353.png]]


### **4.实验结果**

#### Assignment 1

![[Pasted image 20230428153756.png]]

#### Assignment 2 

![[Pasted image 20230501110935.png]]

#### Assignment 3 

1. 新创建的线程如何被调度开始执行

* 初始线程 (pid = 0)

刚进入 `asm_switch_thread()` 的时候：由于第一个进程的调用是在 `setup_kernel()` 中执行的，是通过调用 `asm_switch_thread(0, firstThread)` 如下图所示，可以看到此时sp指向栈中第一个四字节的值是返回地址0x2060a, 第二个值是0即传入的第一个参数，第三个值是0x21d00即第二个传入的参数firstThread的地址，也即第一个线程TCB存储的stack的地址（因为栈在PCB中是第一个成员）

![[Pasted image 20230428204255.png]]

经过几步si调试，程序执行完了取出第一个PCB的task的值到esp中，使得现在栈指针esp指向第一个PCB的栈顶0x22ce4（由上面得到的task的地址0x21d00经过查看内存也可以看到其存储的值就是0x22ce4，即下图在gdb中打印地址的值）；而且打印esp的值也可以看到现在栈指针指向PCB的顶部 - 12，因为经过了4个pop，现在esp上面就是function、program_exit、parameter3个4字节的值

![[Pasted image 20230428205050.png]]
![[Pasted image 20230428205826.png]]

可以看到，经过ret之后就跳转到了上面esp指向的地址0x204b0即 `first_thread()` 的返回地址，自此内核就跳转到了第一个PCB返回的函数执行。由于我还要测试切换线程，所以在该函数里面还会执行创建两个线程的操作，分别返回 `second_threaad()` 和 `third_thread()` 执行

![[Pasted image 20230428210254.png]]


2. 线程的切换和调度

经过了多次时钟中断，此时第一个线程的PCB的ticks为0，调用 `schedule()` 切换线程

![[Pasted image 20230428210947.png]]

执行到了 `asm_switch_thread(cur, next)` 切换线程

![[Pasted image 20230428211601.png]]

此时将 esp 的值保存到第一个PCB中，可以看到PCB的第一个成员stack的值为0x22c48，即当前esp的值

![[Pasted image 20230428211810.png]]

经过栈的切换，可以看到此时esp的值为0x23ce4，刚好为之前esp的值0x22ce4 + 4KB(一个页的大小)，即指向第二个PCB的栈顶。通过打印esp上面的值可以看到先是4个四字节的0，即esi, edi, ebx, ebp四个寄存器，然后是ra即0x20485（`second_thread()` 函数），之后是program_exit, 最后是parameters(为0)

![[Pasted image 20230428212020.png]]

可以看到返回后回到 `second_thread()` 执行第二个PCB的函数

![[Pasted image 20230428212414.png]]


3. 切换回第一个线程

等到所有其他线程都执行结束的时候，此时在线程调度 `schedule()` 和 `asm_switch_thread()` 的时候回到了第一个进程执行，此时执行到了末尾的 `asm_halt` 陷入死循环，至此所有线程结束

![[Pasted image 20230428213135.png]]


以上为从第一个线程创建到执行，再到创建第二个线程和切换第二个线程，最后又切换回到了第一个线程的结尾进行死循环的整个过程

#### Assignment 4

`stop` 代表一个线程暂停；`start` 代表一个线程被调度执行； `exit` 代表一个线程执行结束

**多级反馈队列**：每个进程开始的时候都是10个时间片，处在最高优先级的队列中，之后每次用完所有时间片就被加到下一级队列，优先级降低，时间片增加

**抢占式**：可以看到在第二和第三行虚线之间的打印中看到在pid 0执行的过程中pid 2被加入到调度队列中，且在后面抢占pid 0的线程导致pid 0的线程还有7个时间片就被暂停
 
![[Pasted image 20230501104716.png]]

![[Pasted image 20230501104826.png]]

### 5.  总结

这次实验加深了我对可变参数、printf函数的具体实现；通过gdb的调试查看线程切换前后寄存器等内容的变换，让我对线程调度切换有了更加深刻的了解；同时修改原本的轮转算法为多级反馈队列的可抢占式的算法，也让我更加熟悉线程调度切换的方式，而不只是停留在理论上。
