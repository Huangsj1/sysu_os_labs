; If you meet compile error, try 'sudo apt install gcc-multilib g++-multilib' first

%include "head.include"
; you code here

your_if:
; put your implementation here
  first:
    mov eax, [a1]
    cmp eax, 12
    jge second
    shr eax, 1
    inc eax
    mov [if_flag], eax
    jmp end1
  second:
    cmp eax, 24
    jge third
    mov ebx, 24
    sub ebx, eax
    imul ebx, eax
    mov [if_flag], eax
    jmp end1
  third:
    shl eax, 4
    mov [if_flag], eax
end1:

your_while:
; put your implementation here
mov ebx, [a2]
mov edx, [while_flag]
my_while:
  cmp ebx, 12
  jl end2
  call my_random
  mov esi, ebx
  sub esi, 12
  mov [edx + esi], al
  dec ebx
  jmp my_while
end2:
mov [a2], ebx


%include "end.include"

your_function:
; put your implementation here
mov esi, 0
mov edx, [your_string]
my_for:
  cmp byte [edx + esi], 0
  je end_for
  pushad
  mov al, [edx + esi]
  mov ah, 0
  push ax
  call print_a_char
  pop ax
  popad
  inc esi
  jmp my_for
end_for:
ret
