/** @file getchar.S
 *  @brief Asm wrapper for getchar syscall
 *
 *  @author Ke Wu (kewu)
 *  @author Jian Wang (jianwan3)
 *
 *  @bug No known bugs.
 */

#include <syscall_int.h>

# char getchar(void);

.globl getchar

getchar:
int     $GETCHAR_INT # Syscall of getchar
ret                  # Return from stub
