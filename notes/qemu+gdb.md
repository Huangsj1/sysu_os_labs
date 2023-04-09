>此教程是在：2.15版本的nasm + qemu-systrm-i386 + gdb下的调试

# 基本流程
1. **qemu启动**：在一个terminal启动qemu：  
   qemu-system-i386 -s -S -hda hd.img -serial null -parallel stdio
2. **gdb启动**：在另一个terminal启动gdb并且连接上qemu：  
   gdb  
   target remote:1234
3. **加载符号表**：add-symbol-file xxx.symbol 0xffff
	* 在gdb之前**生成符号表**的操作：
		1. 生成可重定位文件xxx.o：  
		   nasm -o xxx.o -g -f elf32 xxx.asm
		2. 链接生成符号表和可执行文件：  
		   ld -o xxx.symbol -melf_i386 -N xxx.o -Ttext 0xffff  
		   ld -o xxx.bin -melf_i386 -N xxx.o -Ttext 0xffff --oformat binary
1. **设置断点**
2. **运行到断点**
3. **开始调试**

## 简化上述流程

>配合makefile简化上述生成的流程

makefile文件：

```makefile
run:
	qemu-system-i386 -hda hd.img -serial null -parallel stdio 
debug:
	qemu-system-i386 -s -S -hda hd.img -serial null -parallel stdio &
	sleep 1
	gnome-terminal -e "gdb -q -x gdbinit"
build:
	nasm -g -f elf32 mbr.asm -o mbr.o
	ld -o mbr.symbol -melf_i386 -N mbr.o -Ttext 0x7c00
	ld -o mbr.bin -melf_i386 -N mbr.o -Ttext 0x7c00 --oformat binary
	nasm -g -f elf32 bootloader.asm -o bootloader.o
	ld -o bootloader.symbol -melf_i386 -N bootloader.o -Ttext 0x7e00
	ld -o bootloader.bin -melf_i386 -N bootloader.o -Ttext 0x7e00 --oformat binary
	dd if=mbr.bin of=hd.img bs=512 count=1 seek=0 conv=notrunc
	dd if=bootloader.bin of=hd.img bs=512 count=5 seek=1 conv=notrunc
clean:
	rm -fr *.bin *.o *.symbol
```

gdbinit文件

```
target remote:1234
set disassembly-flavor intel
add-symbol-file mbr.symbol 0x7c00
add-symbol-file bootloader.symbol 0x7e00
```

* **执行过程**：
	1. make build
	2. make debug
	3. 接着就可以下断点调试

## gdb常用命令

#### 1. 下断点

* b \*0x7c00：在*地址*下断点
* b function：在*函数名*function下断点（可以是c函数，也可以是汇编函数名）
* b filename:line：在对应*文件的对应行*下断点
* i b：*查看*断点
* d n：*删除*n号断点（配合i b使用）

#### 2. 继续运行

* c：continue，执行到断点
* s：step，单步进入（c）
* n：next，单步跳过（c）
* si：stepi，单步进入（汇编）
* ni：nexti，单步跳过（汇编）

#### 3. 查看内容

1. 查看当前文件、寄存器、内存的值
	* l n：查看**当前文件从第n行**开始
	* i r：查看所有**寄存器**的值
	* x/FMT 0xffff：查看**内存地址的值**（FMT：**单元个数+格式+大小**；**格式**是一个字符，可以是o(octal), x(hex), d(decimal), u(unsigned decimal), t(binary), f(float), a(address), i(instruction), c(char), s(string)；**大小**是一个字符，可以是b(byte, 1 byte), h(halfword, 2 bytes), w(word, 4 bytes), g(giant, 8 bytes)）
2. **layout**查看
	* layout src/r/asm：查看源代码/寄存器值/反汇编后的汇编代码
	* fs src/r/asm：focus到一个窗口（可用上下左右看对应窗口）
	* Ctrl+x a：退出到只剩gdb窗口
	* winheight src/r/asm +/- n：增长/缩短n行当前窗口

参考资料[qemu + gdb](https://gitee.com/nelsoncheung/sysu-2023-spring-operating-system/tree/main/appendix/debug_with_gdb_and_qemu)


