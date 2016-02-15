/** @file mutex_type.h
 *  @brief This file defines the type for mutexes.
 */

#ifndef _MUTEX_TYPE_H
#define _MUTEX_TYPE_H

#include <queue.h>
#include <spinlock.h>

typedef struct mutex {
    int lock_available;
    spinlock_t inner_lock;
    deque_t deque;
} mutex_t;

#endif /* _MUTEX_TYPE_H */
