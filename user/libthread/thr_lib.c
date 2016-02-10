#include <thread.h>
#include "thr_lib_helper.h"

int thread_fork(unsigned int stack_top);
// The amount of stack space available for each thread
static unsigned int stack_size;

// Number of threads created
static unsigned int count;

// Mutex to guard when calculating new stack positions


// Initialize the thread library
int thr_init(unsigned int size) {

    stack_size = size;

    count = 0;

    int isError = 0;

    return isError == 1? -1 : 0;
}

// Create a new thread to run func
int thr_create( void *(*func)(void *), void *args ) {

    // Should use mutex guard before assign stack positions

    int tid;
    count++;

    unsigned int new_stack_top = get_new_stack_top(count, stack_size);

    tid = thread_fork(new_stack_top);

    // Inside original thread
    if(tid < 0) {
        lprintf("thread_fork return < 0 in thr_create"); 
        return tid;
    } else {
        lprintf("old thread returns");
        return 1;
    }

}


