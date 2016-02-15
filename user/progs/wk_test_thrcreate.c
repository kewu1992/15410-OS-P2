#include <stdlib.h>
#include <stdio.h>
#include <simics.h>

#include <syscall.h>
#include <thread.h>
#include <thr_lib_helper.h>

void* func(void* arg) {
    int tid = thr_getid();
    int index = get_stack_position_index();
    lprintf("tid:%d, stack index:%d", tid, index);

    int a = 444;
    if (tid == 2)
        thr_exit((void *)a);

    while(1)
        continue;
}

int main(int argc, char **argv)
{   
    thr_init(4096);

    char buf[] = "Hello, world\n";

    int i;
    for (i = 1; i < 5; i++) {
        thr_create(func, (void *)buf);
        sleep(10);
    }
    
    int status;
    thr_join(2, (void **)&status);
    printf("exit status: %d", status);

    lprintf("master thread reach here");
    while (1)
        continue;
    
    return 0;
}
