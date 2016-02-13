#ifndef _ARRAYTCB_H_
#define _ARRAYTCB_H_

#include <cond_type.h>

typedef enum {
    RUNNING,
    JOINED,
    ZOMBIE
} thr_state_t;

typedef struct {
    int ktid;
    int tid;
    thr_state_t state;
    cond_t cond_var;
} tcb_t;

int arraytcb_init(int size);

int arraytcb_insert_thread(int tid);

int arraytcb_delete_thread(int tid);

tcb_t* arraytcb_get_thread(int index);

int arraytcb_find_thread(int tid);

int arraytcb_set_ktid(int index, int ktid);

void arraytcb_free();

#endif
