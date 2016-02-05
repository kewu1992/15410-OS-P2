
#include <syscall_int.h>

.globl get_ticks

get_ticks:
int $GET_TICKS_INT       
ret              

