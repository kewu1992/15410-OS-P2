/** @file user/progs/small_test.c
 *  @author Jian Wang (jianwan3)
 *  @brief Test syscall stubs
 *  @public yes
 *  @for p2
 *  @covers set_status vanish
 *  @status working
 */

#include <syscall.h>
#include <stdio.h>
#include <simics.h>

int main()
{
    // Though I don't know what 2 is
    set_status(2);

    lprintf("Set status executes");

    vanish();

    return 0;
}

