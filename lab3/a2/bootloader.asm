%include "boot.inc"

;org 0x7e00
;现在仍处于实模式
[bits 16]

;打印bootloader
mov ax, 0xb800
mov gs, ax
mov ah, 0x03
mov ecx, bootloader_tag_end - bootloader_tag
xor ebx, ebx
mov esi, bootloader_tag

output_bootloader_tag:
  mov al, [esi]
  mov word [gs:bx], ax
  inc esi
  add ebx, 2
  loop output_bootloader_tag


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

mov ecx, protect_mode_tag_end - protect_mode_tag
mov ebx, 80 * 2   ;gs的选择子指向VIDEO_SELECTOR,在第二行打印
mov esi, protect_mode_tag
mov ah, 0x3
output_protect_mode_tag:
  mov al, [esi]
  mov word[gs:ebx], ax
  add ebx, 2
  inc esi
  loop output_protect_mode_tag

jmp $

pgdt dw 0
     dd GDT_START_ADDRESS

bootloader_tag db 'run bootloader'
bootloader_tag_end:

protect_mode_tag db 'enter protect mode'
protect_mode_tag_end:
