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
#include <mutex.h>

int main()
{
    mutex_t mp; 
    // Initiate first
    mutex_init(&mp);
    lprintf("mutex inited, lock_available: %d", mp.lock_available);

    mutex_lock(&mp);

    lprintf("mutex locked, lock_available: %d", mp.lock_available);
/*
    lprintf("try lock again");

    mutex_lock(&mp);
    lprintf("Shouldn't reach here");
    lprintf("mutex locked, lock_available: %d", mp.lock_available);
*/

    mutex_unlock(&mp);
    lprintf("mutex unlocked");
    lprintf("try lock again");
    mutex_lock(&mp);
    lprintf("Should reach here");

    return 0;
}

