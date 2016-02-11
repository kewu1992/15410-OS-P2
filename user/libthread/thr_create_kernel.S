#include <syscall_int.h>

.global thr_create_kernel

thr_create_kernel:
    movl    4(%esp), %ecx   # save addr of func
    movl    8(%esp), %edx   # save value of new esp (stack)
    int     $THREAD_FORK_INT# fork a new thread
    testl   %eax, %eax      # check if %eax is zero
    je      .L2             # return value == 0, new thread
    ret                     # return value != 0, original thread
  .L2:              
    movl    %edx, %esp      # set esp to new stack
    call    %ecx            # call func


    