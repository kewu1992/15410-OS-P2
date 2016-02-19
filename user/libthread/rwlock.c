#include <rwlock.h>
#include <mutex.h>
#include <cond.h>
#include <assert.h>
#include <simics.h>
#include <stdio.h>


int rwlock_init( rwlock_t *rwlock ) { 
    rwlock->lock_state = 0;
    rwlock->writer_waiting_count = 0;
    rwlock->reader_waiting_count = 0;

    int is_error = 0;
    is_error |= mutex_init(&rwlock->mutex_inner);
    is_error |= cond_init(&rwlock->cond_reader);
    is_error |= cond_init(&rwlock->cond_writer);
    return is_error ? -1 : 0;
}

void rwlock_lock( rwlock_t *rwlock, int type ) {
    if (type == RWLOCK_READ) {
        mutex_lock(&rwlock->mutex_inner);

        if (rwlock->lock_state == -2) {
            panic("readers/writers lock %p has already been destroied!", rwlock);
        }

        rwlock->reader_waiting_count++;
        while (rwlock->lock_state != 0 || rwlock->writer_waiting_count > 0) {
            if (rwlock->lock_state > 0 && rwlock->writer_waiting_count == 0) 
                break;
            cond_wait(&rwlock->cond_reader, &rwlock->mutex_inner);
        }
        rwlock->reader_waiting_count--;

        rwlock->lock_state++;

        mutex_unlock(&rwlock->mutex_inner);
    } else {
        mutex_lock(&rwlock->mutex_inner);

        if (rwlock->lock_state == -2) {
            panic("readers/writers lock %p has already been destroied!", rwlock);
        }

        rwlock->writer_waiting_count++;
        while (rwlock->lock_state != 0) 
            cond_wait(&rwlock->cond_writer, &rwlock->mutex_inner);
        rwlock->writer_waiting_count--;

        rwlock->lock_state = -1;

        mutex_unlock(&rwlock->mutex_inner);
    }
}

void rwlock_unlock( rwlock_t *rwlock ) {
    mutex_lock(&rwlock->mutex_inner);

    if (rwlock->lock_state == -2) {
        panic("readers/writers lock %p has already been destroied!", rwlock);
    }

    while (rwlock->lock_state == 0) {
        lprintf("try to unlock an unlocked rwlock %p, will wait until it is locked", rwlock);
        printf("try to unlock an unlocked rwlock %p, will wait until it is locked\n", rwlock);
        mutex_unlock(&rwlock->mutex_inner);
        yield(-1);
        mutex_lock(&rwlock->mutex_inner);
    }
    
    rwlock->lock_state = (rwlock->lock_state>0) ? 
                         (rwlock->lock_state-1) : 
                         (rwlock->lock_state+1);
        
    if (rwlock->lock_state == 0) {
        if (rwlock->writer_waiting_count > 0) 
            cond_signal(&rwlock->cond_writer);
        else
            cond_broadcast(&rwlock->cond_reader);
    }

    mutex_unlock(&rwlock->mutex_inner);
}

void rwlock_destroy( rwlock_t *rwlock ) {
    mutex_lock(&rwlock->mutex_inner);
    
    if (rwlock->lock_state == -2) {
        panic("readers/writers lock %p has already been destroied!", rwlock);
    }

    while (rwlock->lock_state != 0 || rwlock->reader_waiting_count != 0 || rwlock->writer_waiting_count != 0) {
        lprintf("Destroy rwlock %p failed, rwlock is locked, will try again...", rwlock);
        printf("Destroy rwlock %p failed, rwlock is locked, will try again...\n", rwlock);
        mutex_unlock(&rwlock->mutex_inner);
        yield(-1);
        mutex_lock(&rwlock->mutex_inner);
    }

    rwlock->lock_state = -2;

    mutex_unlock(&rwlock->mutex_inner);

    mutex_destroy(&rwlock->mutex_inner);
    cond_destroy(&rwlock->cond_reader);
    cond_destroy(&rwlock->cond_writer);
}

void rwlock_downgrade( rwlock_t *rwlock) {
    mutex_lock(&rwlock->mutex_inner);
    if (rwlock->lock_state != -1){
        // illegal
    }
    rwlock->lock_state = 1;
    cond_broadcast(&rwlock->cond_reader);
    mutex_unlock(&rwlock->mutex_inner);
}

