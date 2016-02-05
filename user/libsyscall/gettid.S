
#include <syscall_int.h>

.globl gettid

gettid:
int $GETTID_INT       
ret              

