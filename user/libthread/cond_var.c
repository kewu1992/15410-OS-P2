#include <mutex.h>
#include <thread.h>
#include <syscall.h>
#include <cond_type.h>
#include <stdlib.h>
#include <queue.h>
#include <stdio.h>
#include <thr_internals.h>
#include <simics.h>

int cond_init(cond_t *cv) {
    int is_error = 0;
    is_error |= mutex_init(&cv->mutex);
    is_error |= queue_init(&cv->deque);
    
    return is_error ? -1 : 0;
}

void cond_destroy(cond_t *cv) {
    mutex_lock(&cv->mutex);

    if (!queue_is_active(&cv->deque)) {
        // try to destory a destroied cond_var
        panic("condition variable %p has already been destroied!", cv);
    }

    while (queue_destroy(&cv->deque) < 0) {
        // illegal, some threads are blocked waiting on it
        lprintf("Destroy condition variable %p failed, some threads are blocking on it, will try again...", cv);
        printf("Destroy condition variable %p failed, some threads are blocking on it, will try again...\n", cv);
        mutex_unlock(&cv->mutex);
        yield(-1);
        mutex_lock(&cv->mutex);
    }

    mutex_unlock(&cv->mutex);

    mutex_destroy(&cv->mutex);
        
}

void cond_wait(cond_t *cv, mutex_t *mp) {
    node_t *tmp = malloc(sizeof(node_t));
    while (!tmp) {
        lprintf("malloc failed, will try again...");
        printf("malloc failed, will try again...\n");
        yield(-1);
        tmp = malloc(sizeof(node_t));
    }
    tmp->ktid = thr_getktid();
    tmp->reject = 0;

    mutex_lock(&cv->mutex);

    if (!queue_is_active(&cv->deque)) {
        // try to wait on a destroied cond_var
        panic("condition variable %p has already been destroied!", cv);
    }

    enqueue(&cv->deque, tmp);

    mutex_unlock(mp);

    mutex_unlock(&cv->mutex);
    while(!tmp->reject) {
        if (deschedule(&tmp->reject) < 0) {
            panic("deschedule error of condition variable %p", cv);
        }
    }
    
    free(tmp);

    mutex_lock(mp);
}

void cond_signal(cond_t *cv) {
    mutex_lock(&cv->mutex);
    
    if (!queue_is_active(&cv->deque)) {
        // try to singal a destroied cond_var
        panic("condition variable %p has already been destroied!", cv);
    }

    node_t *tmp = dequeue(&cv->deque);
    if (tmp) {
        int tmp_ktid = tmp->ktid;
        tmp->reject = 1;
        make_runnable(tmp_ktid);
    }
    mutex_unlock(&cv->mutex);
}

void cond_broadcast(cond_t *cv) {
    mutex_lock(&cv->mutex);

    if (!queue_is_active(&cv->deque)) {
        // try to singal a destroied cond_var
        panic("condition variable %p has already been destroied!", cv);
    }

    node_t *tmp = dequeue(&cv->deque);
    while (tmp) {
        int tmp_ktid = tmp->ktid;
        tmp->reject = 1;
        make_runnable(tmp_ktid);
        tmp = dequeue(&cv->deque);
    }
    mutex_unlock(&cv->mutex);
}
