;org 0x7c00
[bits 16]
xor ax,ax

mov ds,ax
mov ss,ax
mov es,ax
mov fs,ax
mov gs,ax

mov sp,0x7d00
mov ax,0xb800
mov ds,ax

mov ah,0x01
mov al,'H'
mov [2*0],ax

mov al,'e'
mov [2*1],ax

mov al,'l'
mov [2*2],ax

mov al,'l'
mov [2*3],ax

mov al,'o'
mov [2*4],ax

mov al,' '
mov [2*5],ax

mov al,'W'
mov [2*6],ax

mov al,'o'
mov [2*7],ax

mov al,'r'
mov [2*8],ax

mov al,'l'
mov [2*9],ax

mov al,'d'
mov [2*10],ax

jmp $

times 510-($-$$) db 0
db 0x55,0xaa
