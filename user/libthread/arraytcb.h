/** @file arraytcb.h
 *  @brief Interfaces for the expandable tcb management array.
 *
 *  @author Ke Wu (kewu)
 *
 *  @bug No known bugs.
 */

#ifndef _ARRAYTCB_H_
#define _ARRAYTCB_H_

#include <cond_type.h>

/** @brief Thread state */
typedef enum {
    RUNNING,
    JOINED
} thr_state_t;

/** @brief Thread control block struct */
typedef struct {
    /** @brief Kernel assigned thread id */
    int ktid;
    /** @brief Thread lib assigned thread id */
    int tid;
    /** @brief Thread state */
    thr_state_t state;
    /** @brief Condition variable that belongs to the thread */
    cond_t cond_var;
} tcb_t;

int arraytcb_init(int size);

int arraytcb_insert_thread(int tid, mutex_t *mutex_arraytcb);

int arraytcb_delete_thread(int index);

tcb_t* arraytcb_get_thread(int index);

tcb_t* arraytcb_find_thread(int tid);

int arraytcb_set_ktid(int index, int ktid);

void arraytcb_free();

int arraytcb_is_valid(int index);

#endif

