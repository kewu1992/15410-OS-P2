#include <syscall_int.h>

# char getchar(void);

.globl getchar

getchar:
    int     $GETCHAR_INT # Syscall of getchar
    ret                  # Return from stub
