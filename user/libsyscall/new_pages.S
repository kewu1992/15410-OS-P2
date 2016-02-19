/** @file new_pages.S
 *  @brief Asm wrapper for new_pages syscall
 *
 *  @author Ke Wu (kewu)
 *  @author Jian Wang (jianwan3)
 *
 *  @bug No known bugs.
 */

#include <syscall_int.h>

# int new_pages(void * addr, int len);

.global new_pages

new_pages:
pushl   %esi
movl    %esp, %esi
addl    $8, %esi
int     $NEW_PAGES_INT
popl    %esi
ret
