#include <syscall_int.h>

# int get_cursor_pos(int *row, int *col);

.global get_cursor_pos

get_cursor_pos:
    pushl   %esi
    movl    %esp, %esi
    addl    $8, %esi
    int     $GET_CURSOR_POS_INT
    popl    %esi
    ret