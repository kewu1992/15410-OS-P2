/** @file misbehave.S
 *  @brief Asm wrapper for misbehave syscall
 *
 *  @author Ke Wu (kewu)
 *  @author Jian Wang (jianwan3)
 *
 *  @bug No known bugs.
 */

#include <syscall_int.h>

# void misbehave(int mode);

.globl misbehave

misbehave:
pushl   %esi          
movl    8(%esp), %esi  
int     $MISBEHAVE_INT 
popl    %esi           
ret                 
