org 0x7c00
[bits 16]

mov ah,0x3
mov bx,0
int 10h

mov ah,0x09
mov al,'!'
mov bh,0
mov bl,0xf4
mov cx,1
int 10h

mov ah,2
mov bh,0
mov dh,7
mov dl,20
int 10h

mov ah,0x3
mov bx,0
int 10h

mov ah,0x9
mov al,'?'
mov bh,0
mov bl,0x94
mov cx,1
int 10h

jmp $

times 510 - ($ - $$) db 0
db 0x55, 0xaa
