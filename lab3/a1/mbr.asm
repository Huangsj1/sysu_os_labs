org 0x7c00
[bits 16]

xor ax, ax
mov ds, ax
mov ss, ax
mov es, ax
mov fs, ax
mov gs, ax

mov sp, 0x7c00

mov ax, 1       ;逻辑扇区号0～15位
mov cx, 0       ;逻辑扇区号16～28位
mov bx, 0x7e00  ;bootloader的加载地址
load_bootloader:
  call asm_read_hard_disk   ;读取并加载bootloader到内存中
  inc ax
  cmp ax, 4
  jle load_bootloader
jmp 0x0000:0x7e00   ;跳转到bootloader

jmp $

;从磁盘读取bootloader到内存中
asm_read_hard_disk:
;参数列表
;ax = 逻辑扇区号0～15位
;cx = 逻辑扇区号16～28位
;ds:bx = 读取出的数据放入的地址，即内存0x7e00

;返回值
;bx = bx+512

;先写入硬盘端口
  mov dx, 0x1f3
  out dx, al      ;LBA地址7～0

  inc dx
  mov al, ah
  out dx, al      ;LBA地址15—～8

  inc dx
  mov ax, cx
  out dx, al      ;LBA地址23～16

  inc dx
  mov al, ah
  and al, 0x0f
  or al, 0xe0
  out dx, al      ;LBA地址27～24

  mov dx, 0x1f2
  mov al, 1
  out dx, al      ;读取一个扇区

  mov dx, 0x1f7
  mov al, 0x20
  out dx, al      ;读命令

;等待直到可用硬盘
.waits:
  in al, dx
  and al, 0x88
  cmp al, 0x08
  jnz .waits

;读取512字节到ds:bx中
  mov cx, 256
  mov dx, 0x1f0
.readw:
  in ax, dx
  mov [bx], ax
  add bx, 2
  loop .readw

  ret

times 510 - ($ - $$) db 0
db 0x55, 0xaa
