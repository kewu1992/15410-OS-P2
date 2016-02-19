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
    int is_error = queue_init(&mp->deque);
    return is_error ? -1 : 0;
}

void mutex_destroy(mutex_t *mp) {
    if (mp->lock_available < 0) {
        // try to destroy a destroied mutex
        panic("mutex %p has already been destroied!", mp);
    }
    while (!SPINLOCK_DESTROY(&mp->inner_lock)) {
        // illegal, mutex is locked
        lprintf("Destroy mutex %p failed, mutex is locked, will try again...", mp);
        printf("Destroy mutex %p failed, mutex is locked, will try again...\n", mp);
        yield(-1);
    }
    while (queue_destroy(&mp->deque) < 0){
        // illegal, some threads are blocked waiting on it
         // illegal, mutex is locked
        lprintf("Destroy mutex %p failed, some threads are blocking on it, will try again...", mp);
        printf("Destroy mutex %p failed, some threads are blocking on it, will try again...\n", mp);
        yield(-1);
    }

    mp->lock_available = -1;
}

void mutex_lock(mutex_t *mp) {
    SPINLOCK_LOCK(&mp->inner_lock);

    if (mp->lock_available < 0) {
        // try to lock a destroied mutex
        panic("mutex %p has already been destroied!", mp);
    }

    if (mp->lock_available){
        mp->lock_available = 0;
        SPINLOCK_UNLOCK(&mp->inner_lock);
    } else {
        node_t *tmp = malloc(sizeof(node_t));
        while (!tmp) {
            lprintf("malloc failed, will try again...");
            printf("malloc failed, will try again...\n");
            yield(-1);
            tmp = malloc(sizeof(node_t));
        }
        tmp->ktid = thr_getktid();
        tmp->reject = 0;

        enqueue(&mp->deque, tmp);
            
        SPINLOCK_UNLOCK(&mp->inner_lock);
        while(!tmp->reject) {
            yield(-1);
        }

        free(tmp);
    }
}

void mutex_unlock(mutex_t *mp) {
    SPINLOCK_LOCK(&mp->inner_lock);

    if (mp->lock_available < 0) {
        // try to unlock a destroied mutex
        panic("mutex %p has already been destroied!", mp);
    }

    while (mp->lock_available == 1) {
        lprintf("try to unlock an unlocked mutex %p, will wait until it is locked", mp);
        printf("try to unlock an unlocked mutex %p, will wait until it is locked\n", mp);
        SPINLOCK_UNLOCK(&mp->inner_lock);
        yield(-1);
    }

    node_t *tmp = dequeue(&mp->deque);

    if (!tmp) {
        mp->lock_available = 1;
        SPINLOCK_UNLOCK(&mp->inner_lock);
    } else {
        int tmp_ktid = tmp->ktid;
        tmp->reject = 1;
        SPINLOCK_UNLOCK(&mp->inner_lock);
        yield(tmp_ktid);
    }
}
