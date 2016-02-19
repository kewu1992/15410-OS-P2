#include <syscall.h>
#include <stdlib.h>
#include <stdio.h>
#include <simics.h>
#include <thread.h>

void* func(void* arg) {
    int tid = thr_getid();  
    if (tid % 2 == 0)
        thr_exit((void*)-tid);  
    else
        return (void*)tid;
    return NULL;
}

int main(int argc, char **argv)
{   
    /*
    thr_init(4096);
    int i;
    for (i = 1; i < 5; i++) {
        thr_create(func, NULL);
    }

    int ret;

    for (i = 1; i < 5; i++) {
        thr_join(i, (void**)&ret);
        lprintf("%d: ret:%d", i, ret);
    }
    */

    return 3;
}
