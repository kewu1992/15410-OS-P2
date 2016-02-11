#include <thread.h>
#include <mutex.h>
#include "thr_lib_helper.h"

int thread_fork(unsigned int stack_top);
// The amount of stack space available for each thread
static unsigned int stack_size;

// Number of threads created
static unsigned int thread_count;

// Mutex to guard when calculating new stack positions
mutex_t mutex_thread_count;

// Initialize the thread library
int thr_init(unsigned int size) {

    stack_size = size;

    mutex_thread_count = 0;

    int isError = 0;

    return isError == 1? -1 : 0;
}

// Create a new thread to run func
int thr_create(void *(*func)(void *), void *args) {
    mutex_lock(&mutex_thread_count);
    int tid = mutex_thread_count++;
    mutex_unlock(&mutex_thread_count);

    uint32_t new_stack = (uint32_t)get_stack_addr(tid);
    memcpy((void*)(new_stack-4), args, 4);

    thr_create_kernel((void*)func, (void*)(new_stack-4));

    return tid;
}

