/** @file task_vanish.S
 *  @brief Asm wrapper for task_vanish syscall
 *
 *  @author Ke Wu (kewu)
 *  @author Jian Wang (jianwan3)
 *
 *  @bug No known bugs.
 */

#include <syscall_int.h>

.globl task_vanish

task_vanish:
pushl %esi         
movl 8(%esp), %esi  
int $TASK_VANISH_INT       
popl %esi            # Should never return and reach here   
ret              

