org 0x7c00
[bits 16]

mov ah,3
mov bx,0
int 0x10

mov ax,0
mov ds,ax
mov si,0
mov cx,8
mov bl,0xf4
mov cx,8

print:
  mov ah,2
  mov bh,0
  int 0x10

  mov ah,9
  mov al,[id+si]
  inc si
  int 0x10

  inc dl
  loop print

id db '2','1','3','0','7','0','3','8'

jmp $
times 510 - ($ - $$) db 0
db 0x55,0xaa
