
#include <syscall_int.h>

# int readfile(char *filename, char *buf, int count, int offset);

.global readfile

readfile:
    pushl   %esi
    movl    %esp, %esi
    addl    $8, %esi
    int     $READFILE_INT
    popl    %esi
    ret