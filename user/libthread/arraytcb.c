
#include <stdlib.h>

#include <cond.h>
#include <arraytcb.h>

static struct arraytcb_s {
    int maxsize, cursize;
    tcb_t** data;
} *array;

int arraytcb_init(int size) {
    if (size <= 0)
        return -1;
    array = malloc(sizeof(struct arraytcb_s));
    array->maxsize = size;
    array->cursize = 0;
    array->data = calloc(size, sizeof(tcb_t*));
    return 0;
}

static void double_array() {
    int newsize = array->maxsize * 2;
    tcb_t** newdata = calloc(newsize, sizeof(tcb_t*));
    
    int i;
    for (i = 0; i < array->cursize; i++)
        newdata[i] = array->data[i];

    array->maxsize = newsize;
    free(array->data);
    array->data = newdata;
}

int arraytcb_insert_thread(unsigned int tid) {
    tcb_t* new_thread = malloc(sizeof(tcb_t));
    new_thread->tid = tid;
    new_thread->state = RUNNING;
    cond_init(&new_thread->cond_var);

    int i;
    for (i = 0; i < array->cursize; i++)
        if (!array->data[i]) {
            array->data[i] = new_thread;
            return i;
        }

    if (array->cursize == array->maxsize)
        double_array(array);

    array->data[array->cursize] = new_thread;
    return array->cursize++;
}

int arraytcb_delete_thread(int tid) {
    int i;
    for (i = 0; i < array->cursize; i++)
        if (array->data[i] && array->data[i]->tid == tid) {
            cond_destroy(&array->data[i]->cond_var);
            free(array->data[i]);
            array->data[i] = NULL;
            return 0;
        }
    return -1;
}

tcb_t* arraytcb_get_thread(int index) {
    if (index >= array->cursize)
        return NULL;
    else
        return array->data[index];
}

int arraytcb_find_thraed(int tid) {
    int i;
    for (i = 0; i < array->cursize; i++)
       if (array->data[i] && array->data[i]->tid == tid)
            return i;
    return -1; 
}

void arraytcv_free() {
     int i;
    for (i = 0; i < array->cursize; i++)
        if (array->data[i]) {
            cond_destroy(&array->data[i]->cond_var);
            free(array->data[i]);
        }
    free(array->data);
    free(array);
}