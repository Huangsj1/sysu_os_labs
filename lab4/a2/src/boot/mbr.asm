%include "boot.inc"

org 0x7c00
[bits 16]

xor ax, ax
mov ds, ax
mov ss, ax
mov es, ax
mov fs, ax
mov gs, ax

mov sp, 0x7c00
mov bp, 0x7c00

mov ax, LOADER_START_SECTOR
mov cx, LOADER_SECTOR_COUNT
mov bx, LOADER_START_ADDRESS

load_bootloader:
  push ax
  push bx
  call asm_read_hard_disk
  add sp, 4
  inc ax
  add bx, 512
  loop load_bootloader

jmp 0x0000:0x7e00

jmp $

;函数asm_read_hard_disk(memo_address, block)
;加载扇区号为block的扇区到内存地址memo_address
asm_read_hard_disk:
  push bp
  mov bp, sp

  push ax
  push bx
  push cx
  push dx

  mov ax, [bp + 2*3]  ;取第二个参数block

;读取扇区
  mov dx, 0x1f3
  out dx, al      ;LBA地址7～0

  inc dx
  mov al, ah
  out dx, al      ;LBA地址15～8

  inc dx
  xor ax, ax
  out dx, al      ;LBA地址23~16

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

;等待磁盘读取可用
 .waits:
  in al, dx
  and al, 0x88
  cmp al, 0x08
  jnz .waits

;读取512字节
  mov bx, [bp + 2*2]
  mov cx, 256
  mov dx, 0x1f0
 .readw:
  in ax, dx
  mov [bx], ax
  add bx, 2
  loop .readw

  pop dx
  pop cx
  pop bx
  pop ax
  pop bp

  ret

times 510 - ($ - $$) db 0
db 0x55, 0xaa
