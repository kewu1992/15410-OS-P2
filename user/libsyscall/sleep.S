
#include <syscall_int.h>

.globl sleep

sleep:
pushl %esi         
movl 8(%esp), %esi  
int $SLEEP_INT       
popl %esi         
ret              

