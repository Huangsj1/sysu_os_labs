# 操作系统原理实验报告

## **实验名称:**   实验七 内存管理

**授课教师：**  张青

**学生姓名:**   黄世杰

**学生学号:**   21307038

### **1.** **实验要求**

完成4个Assignment：Assignment 1 复现参考代码，实现二级分页机制；Assignment 2 参照理论课上的学习的物理内存分配算法如first-fit, best-fit等实现动态分区算法；Assignment 3参照理论课上虚拟内存管理的页面置换算法如FIFO、LRU等，实现页面置换；Assignment 4复现“虚拟页内存管理”一节的代码

### **2.** **实验过程**

#### **Assignment 1**

1. 开启二级页表分页机制
	1. 确定好页目录项和页表在内存中的位置并初始化
	2. 将页目录表的地址写入cr3寄存器中
	3. 将cr0寄存器的PG位置1，此时就开始了分页
2. 物理页面的申请（虚拟页面在A4中）
3. 物理页面的释放

#### Assignment 2

参照理论课上的以及实验指导pdf上的代码，实现物理内存分配算法first-fit：first-fit是指分配第一个适合的内存，做法就是根据遍历bitmap直到找到适合count个物理页面的连续内存来分配

* 优点：设计简单速度较快，找到一个马上就分配
* 缺点：每次查找都会从头开始，而且分配的都是前面的，导致耗时多；当多次分配和释放后可能导致各种已分配的连续内存之间存在内部碎片，不能充分利用

最佳适应算法best-fit虽然减少了内存碎片出现的可能，但每次都要遍历所有的bitmap，较为耗时；而且实现也比first-bit复杂，所以我没有用

![[Pasted image 20230526173828.png]]

#### Assignment 3

页面置换算法是用在物理页面数量不足，但又需要分配的时候，就可以将算法选中的存在的物理页面给换出到磁盘中，并且标记该页表项为换出了的，这时这个物理页面就空闲了，就可以分配给当前所需要用的虚拟页面来映射了

FIFO算法就是先进先出，最先分配的物理页面最先置换出去，其实现方法需要通过新的结构来存储已分配的资源来实现

LRU算法就是最近最少使用，最近最少使用的物理页面最先置换出去，我们可以实现他的简化点的版本：当我们访问一个物理页面的时候CPU就会对其页表项的A位访问位自动置1，我们可以通过一个循环队列来管理所有已分配的物理页帧，当需要置换的时候就循环遍历一遍，如果其A位为0表示最近没访问，就可以置换，否则就将其A位置0，这样下去最多循环一圈就可以置换页表（但是实现起来太复杂）

我这里就只简单尝试了一下FIFO的换出（换入涉及到磁盘交换、页表项修改、页面取消映射等等，很多情况需要考虑）

#### Assignment 4

* 虚拟页内存分配
	1. 从虚拟地址池中分配一定数量的连续的虚拟页
	2. 对分配的每一个虚拟页从物理池中分配一页（不用连续）
	3. 建立虚拟页和物理页的映射
* 虚拟页内存释放
	1. 对每一个虚拟页都要释放为其分配的物理页
	2. 取消所有虚拟页的页表映射
	3. 释放虚拟页

经过测试例子暂时没有发现bug的存在

### **3.关键代码**

#### Assignment  1

1. 开启二级页表分页机制

1. 我们首先将页目录表放在1MB处（PAGE_DIRECTORY），并且初始化页目录表；
2. 然后我们将0~1MB的内核空间在页表中建立直接映射
3. 之后将页目录表的第一个页面的物理页号写入页目录表中建立映射；同时下标为768的页目录表项也指向当前页目录表，方便用户到时候在内核中建立共享区域；最后也要讲最后一个页目录表项指向页目录表，方便得到页目录表虚拟地址和求出对应虚拟地址的页表项
4. 最后调用函数 `asm_init_page_reg()` 初始化cr3（为页目录表的地址）和cr0开启分页机制

![[Pasted image 20230526165230.png]]

![[Pasted image 20230526165953.png]]

2. 物理页面的申请

根据申请的物理页面类型来在对应的内存地址空间中申请连续的count个页面的内存

![[Pasted image 20230526170042.png]]

3. 物理页面的释放

根据传入的物理页面类型来在对应的内存地址空间释放连续的count个页面的内存

![[Pasted image 20230526170107.png]]

#### Assignment 2

首次适应 first-fit 算法：遍历bitmap直到找到满足要求的连续的内存才分配（由于实验手册给的代码也是first-fit，我就简单复现了一遍）

![[Pasted image 20230526172844.png]]

下面是在 `setup_kernel()` 函数的内存管理器初始化后的添加的测试，输出结果符合fitst-fit

![[Pasted image 20230526174806.png]]

#### Assignment 3

1. `BitMap` 中增加了释放最早分配的页面（因为页面的分配是从前往后，所以通常前面的页面都较早分配，我这里简化直接回收前面的页面来腾出空间）

![[Pasted image 20230526181945.png|400]]

![[Pasted image 20230526182328.png|400]]

2. 我修改了 `BitMap::allocate()` 函数，如果没有的分配就先查看是否是count超出了所有的页面长度，是的话就返回-1，否则就释放前面的count个页面分配

![[Pasted image 20230526183148.png|500]]

3. 在 `set_up()` 函数中添加了测试：先分配内核的一半的物理页面，再分配一半，这时候物理页面已经没有了，我这时再分配一半，本来应该报错的，但由于我加了页面换出的程序，能够将前面的最开始分配的那一半物理页面回收（我没有写回磁盘和是实现换入）并空出物理页面来分配

![[Pasted image 20230526183348.png]]

#### Assignment 4

* 虚拟页内存的分配

1. 分配虚拟页

![[Pasted image 20230526184625.png|500]]

2. 分配物理页

![[Pasted image 20230526184809.png|500]]

3. 建立虚拟页面和物理页面的映射关系

![[Pasted image 20230526184854.png]]

总的分配函数

![[Pasted image 20230526184740.png]]

* 虚拟页内存的释放

1. 回收物理页
2. 取消页表映射
3. 回收虚拟页

![[Pasted image 20230526185220.png]]

测试样例的第一个线程：先分配100个页面的空间，再分配10个页面的空间，再分配100个页面的空间；然后释放第二次分配的10个页面的空间，之后再分配100个页面的空间和分配10个页面的空间

![[Pasted image 20230526190323.png]]

### **4.实验结果**

#### Assignment 1

我在开启分页之后还测试了在内存空间中分配和释放5个物理页，可以看到成功执行并正确打印出分配物理页的地址和数目

![[Pasted image 20230526171603.png]]

#### Assignment 2 

我照着上面的first-fit的图先分配了5个连续空间，页面大小分别是5、5、5、3、5，然后释放了第二个和第4个连续空间，最后再分配一个3个页面的空间，可以发现这最后分配的3页面的空间是在原本释放的第二个连续空间的位置分配的，符合first-fit

![[Pasted image 20230526174457.png]]

#### Assignment 3 

如结果显示：分配了两次一半的内存（第一次在0x200000，第二次在0x2138000），然后又分配一次一半的内存，能够将前面的最开始分配的那一半物理页面回收并空出物理页面来分配（最后在0x200000）。

![[Pasted image 20230526183258.png]]

#### Assignment 4

可以看出前面三次分配都没问题，且之后释放了第二次分配的10个页面的空间。对于第四次分配，因为前面释放的10个页面的空间不够，所以在第三次分配的空间的后面再分配100个页面。对于最后一次分配的10个页面的空间，能够在之前的释放的10个页面的空间中分配，说明内存的回收正确，且能够正确实现first-fit的分配函数

![[Pasted image 20230526190340.png]]

### **5. 总结**

本次实验中，通过理论和代码的实践，加深了我对内存的管理的理解：从最开始的直接使用物理内存到通过分页机制实现虚拟内存来将用户和内核隔离开来。同时我也从实际代码中学到了二级分页机制的实现已经CPU是如何将虚拟地址翻译成物理地址（通过MMU翻译）。最后还学到了虚拟地址和物理地址的管理和分配以及二者是如何映射的。