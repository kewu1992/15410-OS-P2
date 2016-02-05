#include <syscall_int.h>

# void misbehave(int mode);

.globl misbehave

misbehave:
    pushl   %esi          
    movl    8(%esp), %esi  
    int     $MISBEHAVE_INT 
    popl    %esi           
    ret                 
