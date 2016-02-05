
#include <syscall_int.h>

// int print(int size, char *buf)

.global print

print:
    movl    %esp, %esi
    addl    $4, %esi
    int     $PRINT_INT
    ret