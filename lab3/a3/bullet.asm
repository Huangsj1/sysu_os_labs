%include "boot.inc"
;org 0xa000
;32位保护模式下运行

mov esp, 0xa000

;清屏
mov ecx, 2000
mov ax, 0
mov si, 0
clear:
  mov [gs:si], ax
  add si, 2
  loop clear

;初始化移动的方向和当前位置
mov dh, 1
mov dl, 1
mov bx, 2
mov cx, 0

mov di, 0

print_and_move:
print:
;先正常打印
  ; 找到内存对应的输出位置
  mov ax, bx
  imul ax, 80
  add ax, cx
  imul ax, 2
  mov si, ax
  ; 打印
  mov al, [es:num + di]
  mov [gs:si], al
  mov al, [es:color + di]
  mov [gs:si+1], al
;再镜像打印
  ;找到镜像位置
  mov ax, bx
  imul ax, 80
  add ax, 79
  sub ax, cx
  imul ax, 2
  mov si, ax
  ; 打印
  mov al, [es:num + di]
  mov [gs:si], al
  mov al, [es:color + di]
  mov [gs:si+1], al

  inc di
  cmp di, 11
  jne end_print
  mov di, 0
end_print:

move:
;先上下移动
  cmp dh, 0
  jne down
up:
  dec bx
  jmp change1
down:
  inc bx
change1:
  cmp bx, -1
  je ch_down
  cmp bx, 25
  je ch_up
  jmp end1
ch_down:
  mov dh, 1
  mov bx, 1
  jmp end1
ch_up:
  mov dh, 0
  mov bx, 23
end1:

;接着左右移动
  cmp dl, 0
  jne right
left:
  dec cx
  jmp change2
right:
  inc cx
change2:
  cmp cx, -1
  je ch_right
  cmp cx, 80
  je ch_left
  jmp end2
ch_right:
  mov dl, 1
  mov cx, 1
  jmp end2
ch_left:
  mov dl, 0
  mov cx, 78
end2:

;延迟
call delay

jmp print_and_move


delay:
  push ecx
  push ax
  mov ecx, 0x8ffff
  delay_loop:
    imul ax, ax
    loop delay_loop
  pop ax
  pop ecx
  ret

num db '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
color db 0x01, 0x10, 0x73, 0x37, 0xa8, 0xb2, 0x5a, 0x81, 0x95, 0x3a


jmp $
