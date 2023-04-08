[bits 32]

global asm_hello_world
global asm_lidt
global asm_unhandled_interrupt
global asm_halt
global asm_div_handler
global asm_time_interrupt_handler
global asm_enable_interrupt
; 输入输出端口
global asm_out_port
global asm_in_port

extern c_time_interrupt_handler

ASM_UNHANDLED_INTERRUPT_INFO db 'Unhandled interrupt happened, halt...'
                             db 0

ASM_DIV_INTERRUPT_INFO db 'You have div 0, interrupt...'
                       db 0

ASM_IDTR dw 0
         dd 0

; 1.打印学号
asm_hello_world:
    push eax
    xor eax, eax

    mov ah, 0x03 ;青色
    mov al, '2'
    mov [gs: 2*0], ax

    mov al, '1'
    mov [gs: 2*1], ax

    mov al, '3'
    mov [gs: 2*2], ax

    mov al, '0'
    mov [gs: 2*3], ax

    mov al, '7'
    mov [gs: 2*4], ax

    mov al, '0'
    mov [gs: 2*5], ax

    mov al, '3'
    mov [gs: 2*6], ax

    mov al, '8'
    mov [gs: 2*7], ax

    pop eax
    ret


; 2.void asm_unhandled_interrupt()：中断处理函数
asm_unhandled_interrupt:
    cli
    mov esi, ASM_UNHANDLED_INTERRUPT_INFO
    xor ebx, ebx
    mov ah, 0x03
.output_information:
    cmp byte[esi], 0
    je .end
    mov al, byte[esi]
    mov word[gs:bx], ax
    inc esi
    add ebx, 2
    jmp .output_information
.end:
    jmp $


; 3.void asm_lidt(uint32 start, uint16 limit)：设置IDTR
asm_lidt:
    push ebp
    mov ebp, esp
    push eax

    mov eax, [ebp + 4 * 3]  ;arg1: limit
    mov [ASM_IDTR], ax
    mov eax, [ebp + 4 * 2]  ;arg1: start
    mov [ASM_IDTR + 2], eax
    lidt [ASM_IDTR]

    pop eax
    pop ebp
    ret


; 4.void asm_halt()：死循环
asm_halt:
    jmp $


; 5.void asm_my_div_handler()
asm_div_handler:
    cli
    mov esi, ASM_DIV_INTERRUPT_INFO
    xor ebx, ebx
    mov ah, 0x4
.output_info:
    cmp byte[esi], 0
    je .end
    mov al, byte[esi]
    mov word[gs:bx], ax
    inc esi
    add ebx, 2
    jmp .output_info
.end:
    jmp $


; 6.void asm_time_interrupt_handler()
asm_time_interrupt_handler:
    pushad

    call c_time_interrupt_handler

    popad

    ; 发送EOI消息，否则下一次中断不发生
    mov al, 0x20
    out 0x20, al
    out 0xa0, al

    iret


; 7.void asm_enable_interrupt()：开启中断
asm_enable_interrupt:
    sti
    ret


; 8.void asm_in_port(uint16 port, uint8* value)
asm_in_port:
    push ebp
    mov ebp, esp

    push edx
    push eax
    push ebx

    xor eax, eax
    mov edx, [ebp + 4 * 2]  ; port
    mov ebx, [ebp + 4 * 3]  ; value

    in al, dx
    mov [ebx], al

    pop ebx
    pop eax
    pop edx
    pop ebp

    ret


; 9.void asm_out_port(uint16 port, uint8 value)
asm_out_port:
    push ebp
    mov ebp, esp

    push edx
    push eax

    mov edx, [ebp + 4 * 2]  ; port
    mov eax, [ebp + 4 * 3]  ; value
    out dx, al

    pop eax
    pop edx
    pop ebp

    ret
