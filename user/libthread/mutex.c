/** @file mutex.S
 *  @brief Implementation of mutex
 *
 *  @author Ke Wu (kewu)
 *  @author Jian Wang (jianwan3)
 *
 *  @bug No known bugs.
 */

#include <mutex.h>
#include <stdlib.h>
#include <syscall.h>
#include <thr_internals.h>
#include <simics.h>
#include <stdio.h>

/** @brief Initialize mutex
 *  
 *  @param mp The mutex to initiate
 *
 *  @return 0 on success; -1 on error
 */
int mutex_init(mutex_t *mp) {
    mp->lock_available = 1; 
    SPINLOCK_INIT(&mp->inner_lock);
    int is_error = queue_init(&mp->deque);
    return is_error ? -1 : 0;
}

/** @brief Destroy mutex
 *  
 *  @param mp The mutex to destory
 *
 *  @return void
 */
void mutex_destroy(mutex_t *mp) {
    SPINLOCK_LOCK(&mp->inner_lock);

    if (mp->lock_available < 0) {
        // try to destroy a destroied mutex
        panic("mutex %p has already been destroied!", mp);
    }

    while (mp->lock_available != 1) {
        // illegal, mutex is locked
        lprintf("Destroy mutex %p failed, mutex is locked, "
                "will try again...", mp);
        printf("Destroy mutex %p failed, mutex is locked, "
                "will try again...\n", mp);
        SPINLOCK_UNLOCK(&mp->inner_lock);
        yield(-1);
        SPINLOCK_LOCK(&mp->inner_lock);
    }

    while (queue_destroy(&mp->deque) < 0){
        // illegal, some threads are blocked waiting on it
        // illegal, mutex is locked
        lprintf("Destroy mutex %p failed, some threads are blocking on it, "
                "will try again...", mp);
        printf("Destroy mutex %p failed, some threads are blocking on it, "
                "will try again...\n", mp);
        SPINLOCK_UNLOCK(&mp->inner_lock);
        yield(-1);
        SPINLOCK_LOCK(&mp->inner_lock);
    }

    mp->lock_available = -1;

    SPINLOCK_UNLOCK(&mp->inner_lock);
}

/** @brief Lock mutex
 *  
 *  A thread will gain exclusive access to the region
 *  after this call if it successfully acquires the lock
 *  until it calles mutex_unlock; or, it will block until
 *  it gets the lock if other thread is holding the lock
 *
 *  @param mp The mutex to lock
 *
 *  @return void
 */
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

/** @brief Unlock mutex
 *  
 *  A thread's exclusive access to the region before this call
 *  till mutex_lock will be lost after this call and other threads
 *  awaiting the lock will have a chance to gain the lock.
 *
 *  @param mp The mutex to unlock
 *
 *  @return void
 */
void mutex_unlock(mutex_t *mp) {
    SPINLOCK_LOCK(&mp->inner_lock);

    if (mp->lock_available < 0) {
        // try to unlock a destroied mutex
        panic("mutex %p has already been destroied!", mp);
    }

    while (mp->lock_available == 1) {
        lprintf("try to unlock an unlocked mutex %p, "
                "will wait until it is locked", mp);
        printf("try to unlock an unlocked mutex %p, "
                "will wait until it is locked\n", mp);
        SPINLOCK_UNLOCK(&mp->inner_lock);
        yield(-1);
        SPINLOCK_LOCK(&mp->inner_lock);
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

