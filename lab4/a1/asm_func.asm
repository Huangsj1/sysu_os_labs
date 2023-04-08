[bits 32]
global function_from_asm
extern function_from_c
extern function_from_cpp

function_from_asm:
    call function_from_c
    call function_from_cpp
    ret
