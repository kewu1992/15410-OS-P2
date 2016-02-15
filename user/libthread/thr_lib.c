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
#include <hashtable.h>

#define INIT_THR_NUM 32
#define EXIT_HASH_SIZE 1021

/** @brief The amount of stack space available for each thread */
static unsigned int stack_size;

/** @brief Number of threads created */
static unsigned int thread_count;

/** @brief Mutex to guard when calculating new stack positions */
mutex_t mutex_thread_count;

mutex_t mutex_arraytcb;

static hashtable_t hash_exit;

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

    // malloc_init must be the first one, because other *_init may use malloc
    isError |= malloc_init();

    isError |= mutex_init(&mutex_thread_count);

    isError |= mutex_init(&mutex_arraytcb);

    isError |= arraytcb_init(INIT_THR_NUM);

    isError |= thr_lib_helper_init(stack_size);

    isError |= thr_hashtableexit_init();

    // insert master thread to arraytcb
    int tmp;
    arraytcb_insert_thread(0, &tmp);
    // set ktid for master thread
    arraytcb_set_ktid(0, gettid());

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

    uint32_t stack_addr = 0;
    int index, is_newstack;

    mutex_lock(&mutex_arraytcb);
    index = arraytcb_insert_thread(tid, &is_newstack);
    mutex_unlock(&mutex_arraytcb);

    if (!is_newstack) {
        stack_addr = get_stack_high(index);
    } else {
        // allocate a stack with stack_size for new thread
        if ((stack_addr = (uint32_t)get_new_stack_top(index)) == -1)
            return -1;
    }

    // "push" argument to new stack  
    memcpy((void*)(stack_addr-4), &args, 4);

    // create a new thread, tell it where it should start running (eip), and
    // its stack address (esp)
    int child_ktid;
    if ((child_ktid = thr_create_kernel(func, (void*)(stack_addr-4))) < 0)
        return -2;

    arraytcb_set_ktid(index, child_ktid);

    //lprintf("tid: %d(%d), stack index: %d, stack addr: %x - %x", tid, child_tid, index, (unsigned int)(new_stack), (unsigned int)(new_stack - stack_size + 1));

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
        // tid can be found in arraytcb, it is still running
        tcb_t* thr = arraytcb_get_thread(index);
        switch(thr->state){
        case JOINED:
            // tid has been joined by other thread
            mutex_unlock(&mutex_arraytcb);
            return -2;
        case RUNNING:
            // tid is still running, block and waiting for it
            thr->state = JOINED;
            //lprintf("tid %d(%d) ---> JOINED", thr->tid, thr->ktid);
            cond_wait(&thr->cond_var, &mutex_arraytcb);
        } 
    } 

    // tid has exitted
    mutex_unlock(&mutex_arraytcb);

    int is_find;
    if (statusp)
        *statusp = hashtable_remove(&hash_exit, (void*)tid, &is_find);
    else
        hashtable_remove(&hash_exit, (void*)tid, &is_find);

    if (is_find)
        return 0;
    else
        // can not find exit status of tid in hash table, something wrong
        return -3;
   
}

void thr_exit(void *status) {

    // Find current thread's stack position index
    int index = get_stack_position_index();

    tcb_t *thr = arraytcb_get_thread(index);
    if(thr == NULL) {
        // Something's wrong
        return;
    }

    //lprintf("thread %d(%d) start exiting", thr->tid, thr->ktid);
    
    hashtable_put(&hash_exit, (void*)(thr->tid), status);

    mutex_lock(&mutex_arraytcb);

    // Thread is either running or someone has called join on it
    if(thr->state == JOINED) {
        // Signal the thread who called join
        // lprintf("thread %d(%d) cond_signal", thr->tid, thr->ktid);
        cond_signal(&thr->cond_var);
    } 

    // release resource
    arraytcb_delete_thread(thr->tid);

    mutex_unlock(&mutex_arraytcb);

    vanish();

    lprintf("should never reach here");
    return;

}

int thr_getid() {

    //mutex_lock(&mutex_arraytcb);

    // Get stack position index of the current thread
    int index = get_stack_position_index();

    tcb_t *tcb = arraytcb_get_thread(index);
    if(tcb == NULL) {
        // Something's wrong, debug
        lprintf("getid fails");
        //mutex_unlock(&mutex_arraytcb);
        return -1;
    }

    //mutex_unlock(&mutex_arraytcb);
    return tcb->tid;
}

int thr_getktid() {
    // Get stack position index of the current thread
    int index = get_stack_position_index();

    tcb_t *tcb = arraytcb_get_thread(index);
    if(tcb == NULL) {
        // Something's wrong, debug
        lprintf("gektid fails");
        return -1;
    }

    return tcb->ktid;
}

int thr_yield(int tid) {
    
    if (tid == -1)
        return yield(-1);

    mutex_lock(&mutex_arraytcb);
    int index = arraytcb_find_thread(tid);
    

    if (index < 0){
        // tid doesn't exist
        mutex_unlock(&mutex_arraytcb);
        return -1;
    }

    tcb_t *tcb = arraytcb_get_thread(index);
    int ktid = tcb->ktid;
    mutex_unlock(&mutex_arraytcb);

    return yield(ktid);

}

int thr_hashtableexit_init() {
    hash_exit.size = EXIT_HASH_SIZE;
    hash_exit.func = thr_exitstatus_hashfunc;
    return hashtable_init(&hash_exit);
}

int thr_exitstatus_hashfunc(void *key) {
    int tid = (int)key;
    return tid % EXIT_HASH_SIZE;
}

