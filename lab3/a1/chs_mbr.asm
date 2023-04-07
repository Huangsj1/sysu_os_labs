org 0x7c00
[bits 16]

xor ax, ax
mov ds, ax
mov ss, ax
mov es, ax
mov fs, ax
mov gs, ax

mov sp, 0x7c00

;从磁盘读取bootloader到内存中
;用HCS来读取：int 13H
asm_read_hard_disk:
  mov bx, 0x7e00
  mov ah, 2
  mov al, 5
  mov ch, 0
  mov dh, 0
  mov cl, 2
  mov dl, 80h
  int 13h
jmp 0x0000:0x7e00

jmp $

times 510 - ($ - $$) db 0
db 0x55, 0xaa
