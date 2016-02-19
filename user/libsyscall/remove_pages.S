/** @file remove_pages.S
 *  @brief Asm wrapper for remove_pages syscall
 *
 *  @author Ke Wu (kewu)
 *  @author Jian Wang (jianwan3)
 *
 *  @bug No known bugs.
 */

#include <syscall_int.h>

# int remove_pages(void * addr);

.globl remove_pages

remove_pages:
pushl   %esi          
movl    8(%esp), %esi  
int     $REMOVE_PAGES_INT 
popl    %esi           
ret                 
