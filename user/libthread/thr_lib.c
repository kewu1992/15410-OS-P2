/** @file thr_lib.c
 *  @brief This file contains implementation of thread management library 
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <thread.h>
#include <mutex.h>
#include <thr_lib_helper.h>
#include <thr_internals.h>
#include <arraytcb.h>

#define INIT_THR_NUM 32

/** @brief The amount of stack space available for each thread */
static unsigned int stack_size;

/** @brief Number of threads created */
static unsigned int thread_count;

/** @brief Mutex to guard when calculating new stack positions */
mutex_t mutex_thread_count;

/** @brief Initialize the thread library
 *
 *  @param size The amount of stack space which will be available for each 
                thread using the thread library
 *  @return On success return zero, on error return a negative number
 */
int thr_init(unsigned int size) {

    stack_size = size;

    // At first, already has a master thread
    thread_count = 1;

    int isError = 0;

    isError |= mutex_init(&mutex_thread_count);

    isError |= arraytcb_init(INIT_THR_NUM);

    return isError ? -1 : 0;
}

/** @brief Creates a new thread to run func(args)
 *  
 *  This function will create a thread (a register set and a stack) to
 *  run func(args). 
 *
 *  @param func The address of function for new thread to run
 *  @param args The argument that passed to the function 
 *              for new thread to run  
 *  @return On success the thread ID of the new thread is returned, on error a 
 *          negative number is returned
 */
int thr_create(void *(*func)(void *), void *args) {
    mutex_lock(&mutex_thread_count);
    int tid = thread_count++;
    mutex_unlock(&mutex_thread_count);

    // allocate a stack with stack_size for new thread
    uint32_t new_stack;
    if ((new_stack = (uint32_t)get_new_stack_top(tid, stack_size)) == -1)
        return -1;

    // "push" argument to new stack  
    memcpy((void*)(new_stack-4), &args, 4);

    // create a new thread, tell it where it should start running (eip), and
    // its stack address (esp)
    if (thr_create_kernel(func, (void*)(new_stack-4)) < 0)
        return -2;

    return tid;
}

/*
int thr_join(int tid, void **statusp) {
    // tid was not created 
    if (tid > thread_count)
        return -1;
    
    int index = arraylist_find(array, tid);
    if (index >= 0) {
        // tid has not exited

        
    } else {
        // tid may has exited, try to find -tid
        index = arraylist_find(array, -tid);
        // tid can not be found, tid has already been cleaned up
        if (index < 0)
            return -2;

    }
}
*/

int thr_getid() {
    return 0;
}

