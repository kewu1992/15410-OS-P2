
#include <syscall_int.h>

.globl task_vanish

task_vanish:
pushl %esi         
movl 8(%esp), %esi  
int $TASK_VANISH_INT       
popl %esi            # Should never return and reach here   
ret              

