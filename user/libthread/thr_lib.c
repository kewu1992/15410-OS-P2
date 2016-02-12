/** @file thr_lib.c
 *  @brief This file contains implementation of thread management library 
 *
 *  @bug Better use spin lock for mutex_thread_count
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <thread.h>
#include <mutex.h>
#include <cond.h>
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

mutex_t mutex_arraytcb;

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

    isError |= malloc_init();

    isError |= mutex_init(&mutex_arraytcb);

    isError |= arraytcb_init(INIT_THR_NUM);

    isError |= thr_lib_helper_init(stack_size);

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
    if ((new_stack = (uint32_t)get_new_stack_top(tid)) == -1)
        return -1;

    // "push" argument to new stack  
    memcpy((void*)(new_stack-4), &args, 4);

    // create a new thread, tell it where it should start running (eip), and
    // its stack address (esp)
    if (thr_create_kernel(func, (void*)(new_stack-4)) < 0)
        return -2;

    return tid;
}


int thr_join(int tid, void **statusp) {
    // check if tid has been created 
    mutex_lock(&mutex_thread_count);
    if (tid >= thread_count){
        mutex_unlock(&mutex_thread_count);
        return -1;
    }
    mutex_unlock(&mutex_thread_count);

    mutex_lock(&mutex_arraytcb);
    int index = arraytcb_find_thread(tid);
    if (index >= 0) {
        tcb_t* thr = arraytcb_get_thread(index);
        switch(thr->state){
            case JOINED:
                // tid has been joined by other thread
                mutex_unlock(&mutex_arraytcb);
                return -2;
            case RUNNING:
                // tid is still running, block and waiting for it
                thr->state = JOINED;
                cond_wait(&thr->cond_var, &mutex_arraytcb);
                // after returning from cond_wait(), tid becoms ZOMBIE,
                // fall through ZOMBIE case
            case ZOMBIE:
                // tid has exitted

                if (statusp) {
                    // get exit status
                }

                // release resource
                arraytcb_delete_thread(tid);
        } 

        mutex_unlock(&mutex_arraytcb);
        return 0;
    } else {
        // tid can not be found, tid has already been cleaned up
        mutex_unlock(&mutex_arraytcb);
        return -2;
    }
}

void thr_exit(void *status) {

    // Get current thread tid
    int tid = thr_getid();

    // When thr_create is called at this point
    // should be fine ... 
    mutex_lock(&mutex_arraytcb);
    // Find current thread's stack position index
    int index = arraytcb_find_thread(tid);
    if(index == -1) {
        // Something's wrong
        return;
    }
    // Get the tcb of tid
    tcb_t *thr = arraytcb_get_thread(index);
    if(thr == NULL) {
        // Something's wrong
        return;
    }

    if(thr->state == ZOMBIE) {
        // This thread has exited before, should never happen
        lprintf("Thread has called thr_exit() before, something's wrong");
        return;
    }

    // Thread is either running or someone has called join on it
    // Get stack high of current thread
    uint32_t cur_stack_high = get_stack_high(index);
    lprintf("cur_stack_hign: %u", (unsigned)cur_stack_high);

    // Push status on current stack high to let it be collected by a
    // thread who joins this thread
    memcpy((void*)(cur_stack_high - sizeof(void *)), &status, 
            sizeof(void *));

    if(thr->state == RUNNING) {
        // Mark current thread as ZOMBIE
        thr->state = ZOMBIE;
    } else if(thr->state == JOINED) {
        // Signal the thread who called join
        cond_signal(&thr->cond_var);
    }

    mutex_unlock(&mutex_arraytcb);
    vanish();
    return;

}

int thr_getid() {

    mutex_lock(&mutex_arraytcb);

    // Get stack position index of the current thread
    int index = get_stack_position_index();

    tcb_t *tcb = arraytcb_get_thread(index);
    if(tcb == NULL) {
        // Something's wrong, debug
        lprintf("getid fails");
        return -1;
    }

    mutex_unlock(&mutex_arraytcb);
    return tcb->tid;
}

