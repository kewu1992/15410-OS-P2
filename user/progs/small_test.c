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
#include <thread.h>

mutex_t mp;

void* func(void* arg) {
    lprintf("tid %d try to get lock", thr_getid());
    mutex_lock(&mp);
    
    lprintf("tid %d has got lock", thr_getid());
    int i;
    for (i = 0; i < 10; i++){
        lprintf("tid %d count down: %d", thr_getid(), 10-i);
        sleep(10);
    }
    lprintf("tid %d ready to realse lock", thr_getid());

    mutex_unlock(&mp);
    return 0;
}

int main()
{   
    thr_init(4096);

    mutex_init(&mp);
    mutex_lock(&mp);

    lprintf("master thread has got lock");

    int tid1 = thr_create(func, NULL);
    int tid2 = thr_create(func, NULL);

    int i;
    for (i = 0; i < 10; i++){
        lprintf("master count down: %d", 10-i);
        sleep(10);
    }
    

    lprintf("master thread, ready to release lock");    
    mutex_unlock(&mp);

    thr_join(tid1, NULL);
    thr_join(tid2, NULL);

    lprintf("master thread, ready to destroy lock");    

    mutex_destroy(&mp);

    return 0;
}

