org 0x7c00
[bits 16]

; get cursor place
mov ah, 03h
mov bx, 0
int 10h

; set color and number wu will print 
mov bl, 0x74
mov cx, 1

get_and_print:
  ; get the char
  mov ah, 0
  int 16h

  ; print the char
  mov ah, 09h
  int 10h

  ; move the cursor
  mov ah, 02h
  inc dl
  int 10h

  jmp get_and_print

times 510 - ($ - $$) db 0
db 0x55, 0xaa
