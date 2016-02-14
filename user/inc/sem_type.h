/** @file sem_type.h
 *  @brief This file defines the type for semaphores.
 */

#ifndef _SEM_TYPE_H
#define _SEM_TYPE_H

#include <mutex.h>
#include <cond.h>
#include <stddef.h>

typedef struct sem {
    mutex_t mutex;
    cond_t cond;
    int count;
} sem_t;

#endif /* _SEM_TYPE_H */
