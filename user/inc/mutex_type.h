/** @file mutex_type.h
 *  @brief This file defines the type for mutexes.
 */

#ifndef _MUTEX_TYPE_H
#define _MUTEX_TYPE_H


typedef struct mutex {
    int lock_available;
} mutex_t;

#endif /* _MUTEX_TYPE_H */
