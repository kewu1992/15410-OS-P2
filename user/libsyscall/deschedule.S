
#include <syscall_int.h>

.globl deschedule

deschedule:
pushl %esi         
movl 8(%esp), %esi  
int $DESCHEDULE_INT       
popl %esi         
ret              

