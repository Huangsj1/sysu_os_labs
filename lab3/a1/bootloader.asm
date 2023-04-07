org 0x7e00
[bits 16]

mov ax, 0xb800
mov gs, ax
xor ax, ax
mov ds, ax

mov ah, 0x03
mov cx, bootloader_tag_end - bootloader_tag
xor bx, bx
mov si, bootloader_tag

output_bootloader_tag:
  mov al, [si]
  mov word[gs:bx], ax
  inc si
  add bx, 2
  loop output_bootloader_tag
jmp $

bootloader_tag db 'run bootloader'
bootloader_tag_end:
