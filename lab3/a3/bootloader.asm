%include "boot.inc"

;org 0x7e00
;现在仍处于实模式
[bits 16]

;现在准备进入保护模式
;1.填充GDT
;空描述符
mov dword [GDT_START_ADDRESS+0x00], 0x00
mov dword [GDT_START_ADDRESS+0x04], 0x00

;数据段描述符
mov dword [GDT_START_ADDRESS+0x08], 0x0000ffff
mov dword [GDT_START_ADDRESS+0x0c], 0x00cf9200

;堆栈段描述符
mov dword [GDT_START_ADDRESS+0x10], 0x00000000
mov dword [GDT_START_ADDRESS+0x14], 0x00409600

;显存描述符
mov dword [GDT_START_ADDRESS+0x18], 0x80007fff
mov dword [GDT_START_ADDRESS+0x1c], 0x0040920b

;代码段描述符
mov dword [GDT_START_ADDRESS+0x20], 0x0000ffff
mov dword [GDT_START_ADDRESS+0x24], 0x00cf9800

;初始化描述符表寄存器GDTR
mov word [pgdt], 39
lgdt[pgdt]


;2.打开第21根地址线
in al, 0x92
or al, 0000_0010B
out 0x92, al


;3.进入保护模式
cli     ;关中断
mov eax, cr0
or eax, 1
mov cr0, eax


;4.跳转到32位保护模式的代码
;注意：这里改变了cs为CODE_SELECTOR
jmp dword CODE_SELECTOR:protect_mode_begin


;清除流水线并且串行化处理器
[bits 32]
protect_mode_begin:

;加载选择子到段寄存器中
mov eax, DATA_SELECTOR
mov ds, eax
mov es, eax
mov eax, STACK_SELECTOR
mov ss, eax
mov eax, VIDEO_SELECTOR
mov gs, eax

;加载6号磁盘（存储着我的汇编代码）到内存0xa000处执行
mov ax, 6

mov dx, 0x1f3
out dx, al

inc dx
mov al, ah
out dx, al

inc dx
xor ax, ax
out dx, al

inc dx
mov al, ah
and al, 0x0f
or al, 0xe0
out dx, al

mov dx, 0x1f2
mov al, 1
out dx, al

mov dx, 0x1f7
mov al, 0x20
out dx, al

;等待磁盘可用
.waits:
  in al, dx
  and al, 0x88
  cmp al, 0x08
  jnz .waits

;读取512字节
mov bx, 0xa000
mov dx, 0x1f0
mov cx, 256
.readw:
  in ax, dx
  mov [ds:bx], ax
  add bx, 2
  loop .readw

jmp 0xa000

jmp $

pgdt dw 0
     dd GDT_START_ADDRESS
