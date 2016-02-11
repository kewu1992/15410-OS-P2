/** @file cond_type.h
 *  @brief This file defines the type for condition variables.
 */

#ifndef _COND_TYPE_H
#define _COND_TYPE_H

#include <mutex_type.h>
#include <queue.h>


typedef struct cond {
    mutex_t mutex;
    node_t queue_head, queue_tail;
} cond_t;

#endif /* _COND_TYPE_H */
