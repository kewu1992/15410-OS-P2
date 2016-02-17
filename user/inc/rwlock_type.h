/** @file rwlock_type.h
 *  @brief This file defines the type for reader/writer locks.
 */

#ifndef _RWLOCK_TYPE_H
#define _RWLOCK_TYPE_H

#include <mutex_type.h>
#include <cond_type.h>

typedef struct rwlock {
    int lock_state;
    int writer_waiting_count;
    mutex_t mutex_inner;
    cond_t  cond_reader;
    cond_t  cond_writer;
} rwlock_t;

#endif /* _RWLOCK_TYPE_H */
