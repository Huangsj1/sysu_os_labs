%include "boot.inc"

org 0x7e00
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

; 加载内核
mov eax, KERNEL_START_SECTOR
mov ebx, KERNEL_START_ADDRESS
mov ecx, KERNEL_SECTOR_COUNT

load_kernel:
    push eax
    push ebx
    call asm_read_hard_disk  ; 读取硬盘
    add esp, 8
    inc eax
    add ebx, 512
    loop load_kernel

jmp dword CODE_SELECTOR:KERNEL_START_ADDRESS       ; 跳转到kernel

jmp $ ; 死循环

; asm_read_hard_disk(memory,block)
; 加载逻辑扇区号为block的扇区到内存地址memory

asm_read_hard_disk:
    push ebp
    mov ebp, esp

    push eax
    push ebx
    push ecx
    push edx

    mov eax, [ebp + 4 * 3] ; 逻辑扇区低16位

    mov edx, 0x1f3
    out dx, al    ; LBA地址7~0

    inc edx        ; 0x1f4
    mov al, ah
    out dx, al    ; LBA地址15~8

    xor eax, eax
    inc edx        ; 0x1f5
    out dx, al    ; LBA地址23~16 = 0

    inc edx        ; 0x1f6
    mov al, ah
    and al, 0x0f
    or al, 0xe0   ; LBA地址27~24 = 0
    out dx, al

    mov edx, 0x1f2
    mov al, 1
    out dx, al   ; 读取1个扇区

    mov edx, 0x1f7    ; 0x1f7
    mov al, 0x20     ;读命令
    out dx,al

    ; 等待处理其他操作
  .waits:
    in al, dx        ; dx = 0x1f7
    and al,0x88
    cmp al,0x08
    jnz .waits

    ; 读取512字节到地址ds:bx
    mov ebx, [ebp + 4 * 2]
    mov ecx, 256   ; 每次读取一个字，2个字节，因此读取256次即可
    mov edx, 0x1f0
  .readw:
    in ax, dx
    mov [ebx], eax
    add ebx, 2
    loop .readw

    pop edx
    pop ecx
    pop ebx
    pop eax
    pop ebp

    ret

pgdt dw 0
     dd GDT_START_ADDRESS
