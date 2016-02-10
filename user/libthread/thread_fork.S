#include <syscall_int.h>

.globl thread_fork

thread_fork:
movl    4(%esp), %ecx   # Put new thread stack top addr in %ecx
movl    8(%ebp), %edx   # Put func in edx
subl    $4, %ecx        # Leave space to put func on new stack
movl    %edx, (%ecx)    # Put func on new stack
movl    12(%ebp), %edx  # Put args in edx 
subl    $4, %ecx        # Leave space to put args on new stack
movl    %edx, (%ecx)    # Put args on new stack

int     $THREAD_FORK_INT

# Now we have two identical register sets
cmpl    $0, %eax
jz      .INVOKED_THREAD

# Return value is nozero, inside invoking thread
ret                     # Return

.INVOKED_THREAD:
# Return value is 0, inside invoked thread
movl    %ecx, %esp      # New thread stack top 
# Now the new stack would look like
# func    
# args    <-- %esp 
movl    4(%esp), %ecx   # Put func in %ecx
call    %ecx

