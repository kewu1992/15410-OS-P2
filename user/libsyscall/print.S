
#include <syscall_int.h>

// int print(int size, char *buf)

.global print

print:
    pushl   %esi
    movl    %esp, %esi
    addl    $8, %esi
    int     $PRINT_INT
    popl    %esi
    ret