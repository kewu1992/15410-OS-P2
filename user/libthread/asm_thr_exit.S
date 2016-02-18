#include <syscall_int.h>

.global asm_thr_exit

asm_thr_exit:
    movl    4(%esp), %ecx   # %ecx = &mutex_arraytcb->inner_lock
    movl    $1, %eax        # %eax = 1
    xchg    (%ecx), %eax    # atomically do mutex_arraytcb->inner_lock = 1
    int     $VANISH_INT     # Syscall of vanish
    ret                     # should never reach here though
