
#include <syscall_int.h>

.globl wait

wait:
pushl %esi         
movl 8(%esp), %esi  
int $WAIT_INT       
popl %esi         
ret              

