#include <stdlib.h>
#include <stdio.h>
#include <simics.h>

#include <syscall.h>
#include <thread.h>

void* func(void* arg) {
    lprintf("new thread reach here");
    char* str = (char*) arg;
    print(13, str);
    while (1)
        continue;
}

int main(int argc, char **argv)
{   
    thr_init(4096);

    char buf[] = "Hello, world\n";

    thr_create(func, (void *)buf);
    
    lprintf("master thread reach here");
    while (1)
        continue;
    
    return 0;
}
