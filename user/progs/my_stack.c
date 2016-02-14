/** @file 410user/progs/stack_test1.c
 *  @author efaust
 *  @brief Tests whether auto-stack pages are zeroed.
 *  @public yes
 *  @for p3
 *  @covers autostack
 *  @status done
 *
 *  Note: If we ever go back to the kernel-level autostack, we should revert
 *  this to the old one.
 */

#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>
#include "410_tests.h"
#include <report.h>
#include <thread.h>


DEF_TEST_NAME("stack_test1:");

#define SIZE (65536 * 3)

void *
waiter(void *p)
{
    int status;

    thr_join((int)p, (void **)&status);
    printf("Thread %d exited '%c'\n", (int) p, (char)status);

    thr_exit((void *) 0);

    while(1)
        continue; /* placate compiler portably */
}


char hall() {
    // Should fail at this case
    char ca2[SIZE];
    int i;
    for(i = 0; i < SIZE; i++) {
        ca2[i] = i;
    }

    return ca2[0];
}

char touch_me_babe()
{

    thr_init(PAGE_SIZE);

    (void) thr_create(waiter, (void *) thr_getid());

    // Test autostack after creating a new thread, should fail
    return hall();
}

void do_test()
{
    touch_me_babe();
}

int main()
{

    report_start(START_CMPLT);

    do_test();
    report_misc("stack allocation successful");

    thr_exit((void *)'!');

    while(1)
        continue; /* placate compiler portably */
    exit(42);
}

