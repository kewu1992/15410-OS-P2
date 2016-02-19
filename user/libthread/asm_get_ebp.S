/* define asm_get_ebp label */
.global asm_get_ebp


asm_get_ebp:
    movl    %ebp,%eax
    ret
