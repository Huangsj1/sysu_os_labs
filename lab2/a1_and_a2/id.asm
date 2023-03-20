org 0x7c00
[bits 16]

xor ax,ax

mov ds,ax
mov ss,ax
mov es,ax
mov fs,ax
mov gs,ax

mov sp,0x7d00
mov ax,0xb800
add ax,1
mov gs,ax

mov si,0
mov di,0
mov cx,8

copy_id: 
  mov al,[idnum+si]
  mov ah,[idcol+si]
  mov [gs:di+1944],ax
  inc si
  add di,2
  loop copy_id

jmp $

idnum db '2','1','3','0','7','0','3','8'
idcol db 0x71,0xf9,0x72,0xfa,0x73,0xfb,0x74,0xfc

times 510-($-$$) db 0
db 0x55,0xaa

