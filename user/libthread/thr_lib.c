
#include <mutex.h>

int thread_n;
mutex_t mutex_thread_n;

int thr_create(void *(*func)(void *), void *args) {
    mutex_lock(&mutex_thread_n);
    int tid = ++thread_n;
    mutex_unlock(&mutex_thread_n);

    uint32_t new_stack = (uint32_t)get_stack_addr(tid);
    memcpy((void*)(new_stack-4), args, 4);

    thr_create_kernel((void*)func, (void*)(new_stack-4));

    return tid;
}