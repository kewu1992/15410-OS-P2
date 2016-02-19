#include <syscall.h>
#include <stdlib.h>
#include <stdio.h>
#include <simics.h>
#include <thread.h>
#include <mutex.h>
#include <cond.h>
#include <thread.h>
#include <sem.h>
#include <rwlock.h>

cond_t test;
mutex_t test2;
sem_t sem;

void* func(void* arg) {
    int tid = thr_getid();  
    if (tid % 2 == 0)
        thr_exit((void*)-tid);  
    else
        return (void*)tid;
    return NULL;
}

void* func2(void* arg) {
    mutex_lock(&test2);
    cond_wait(&test, &test2);
    return NULL;
}

void* func3(void* arg) {
    sleep(5);
    cond_signal(&test);
    return NULL;
}

void* func4(void* arg) {

    sem_wait(&sem);
    sem_wait(&sem);

    return NULL;
}

int main(int argc, char **argv)
{   
    thr_init(4096);
    rwlock_t rwlock;
    rwlock_init(&rwlock);
    rwlock_destroy(&rwlock);
    rwlock_destroy(&rwlock);
    
    return 0;
}
