/*
 * @bug 1. one thread lock twice?
 *      2. mutex_destroy_test?  
 */
#include <mutex.h>
#include <stdlib.h>
#include <syscall.h>
#include <thr_internals.h>
#include <simics.h>

int mutex_init(mutex_t *mp) {
    mp->lock_available = 1; 
    SPINLOCK_INIT(&mp->inner_lock);
    queue_init(&mp->deque);
    return 0;
}

void mutex_destroy(mutex_t *mp) {
    if (!SPINLOCK_DESTROY(&mp->inner_lock)) {
        // illegal, mutex is locked
    }
    if (queue_destroy(&mp->deque) < 0){
        // illegal, some threads are blocked waiting on it
    }
}

void mutex_lock(mutex_t *mp) {
    SPINLOCK_LOCK(&mp->inner_lock);

    if (mp->lock_available){
        mp->lock_available = 0;
        SPINLOCK_UNLOCK(&mp->inner_lock);
    } else {
        node_t *tmp = malloc(sizeof(node_t));
        tmp->ktid = thr_getktid();
        tmp->reject = 0;

        enqueue(&mp->deque, tmp);
            
        SPINLOCK_UNLOCK(&mp->inner_lock);
        while(!tmp->reject) {
            deschedule(&tmp->reject);
        }

        free(tmp);
    }
}

void mutex_unlock(mutex_t *mp) {
    SPINLOCK_LOCK(&mp->inner_lock);

    node_t *tmp = dequeue(&mp->deque);

    if (!tmp) {
        mp->lock_available = 1;
    } else {
        int tmp_ktid = tmp->ktid;
        tmp->reject = 1;
        make_runnable(tmp_ktid);
    }
    SPINLOCK_UNLOCK(&mp->inner_lock);
}
