#include <syscall_int.h>

# void halt();

.globl halt

halt:
    int     $HALT_INT    # Syscall of halt
    ret                  # Return from stub