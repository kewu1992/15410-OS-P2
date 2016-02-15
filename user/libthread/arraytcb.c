/** @file arraytcb.c
 *  @brief This file contains implementation of arraytcb
 *
 *  Arraytcb is a data structure to maintain metadata of user-level threads.
 *  Each element in the array of arraytcb represents a thread, it contains 
 *  information such as tid, thread state, etc. The index of the array
 *  indicates the stack number that the thread is running on. For example,
 *  array[3]->data->tid == 2 means thread #2 is running on stack #3. At 
 *  beginning, master thread is running on stack 0. array[i] == NULL means
 *  stack i is not used by any thread.
 *
 *
*  @bug array shouldn't always be extended --> watch out malloc error 
*       (not enough memory, VM/PM)
 */

#include <stdlib.h>

#include <cond.h>
#include <arraytcb.h>

static struct arraytcb_s {
    int maxsize, cursize;
    tcb_t** data;
} *array;

/** @brief Initialize arraytcb data structure
 *  
 *  @param size The initial size of arraytcb
 *
 *  @return On success return 0, on error return -1
 *
 */
int arraytcb_init(int size) {
    if (size <= 0)
        return -1;
    array = malloc(sizeof(struct arraytcb_s));
    array->maxsize = size;
    array->cursize = 0;
    array->data = calloc(size, sizeof(tcb_t*));
    return 0;
}

/** @brief Double the size of arrarytcb
 */
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

/** @brief Insert a thread (indicated by tid) to arraytcb
 *  
 *  Initialize a tcb structure for the thread.
 *  The program will first check if there is any existing stack that is 
 *  available, if not it will allocate a new stack slot for the thread.
 *  
 *  @param tid The tid of the new thread that need to be inserted
 *  @param is_newstack Also a return value, indicate if a new stack slot is used
 *                     for the new thread
 *
 *  @return The index of the stack that is used for the new thread
 *          
 *
 */
int arraytcb_insert_thread(int tid, int *is_newstack) {
    tcb_t* new_thread = malloc(sizeof(tcb_t));
    new_thread->tid = tid;
    new_thread->state = RUNNING;
    cond_init(&new_thread->cond_var);

    int i;
    for (i = 0; i < array->cursize; i++)
        if (!array->data[i]) {
            array->data[i] = new_thread;
            *is_newstack = 0;
            return i;
        }

    if (array->cursize == array->maxsize)
        double_array(array);

    array->data[array->cursize] = new_thread;
    *is_newstack = 1;
    return array->cursize++;
}

/** @brief Delete a thread from arraytcb
 *  
 *  After deletion, the stack that belonged to the deleted thread becomes 
 *  available for other threads to use.
 *  
 *  @param tid The tid for the thread that need to be deleted
 *
 *  @return On success return 0, on error return -1
 *
 */
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

/** @brief Get the tcb structure given an array index
 *  
 *  @param index Index of array to get tcb structure
 *
 *  @return Pointer points to the tcb structure with index
 *
 */
tcb_t* arraytcb_get_thread(int index) {
    if (index >= array->cursize)
        return NULL;
    else
        return array->data[index];
}

/** @brief Find the index of a given thraed
 *  
 *  @param tid The tid of the thread that need to find its index
 *
 *  @return On success return the index of the thread, on error return
 *          -1 (can not find the thread in arraytcb)
 *
 */
int arraytcb_find_thread(int tid) {
    int i;
    for (i = 0; i < array->cursize; i++)
       if (array->data[i] && array->data[i]->tid == tid)
            return i;
    return -1; 
}

int arraytcb_set_ktid(int index, int ktid) {
    if (index >= array->cursize || !array->data[index])
        return -1;

    array->data[index]->ktid = ktid;
    
    return 0;
}

/** @brief Free arraytcb data structure, release resource
 *  
 *  Probably this function should never be called...
 *
 */
void arraytcb_free() {
     int i;
    for (i = 0; i < array->cursize; i++)
        if (array->data[i]) {
            cond_destroy(&array->data[i]->cond_var);
            free(array->data[i]);
        }
    free(array->data);
    free(array);
}
