/** @file queue.h
 *  @brief Function prototypes of a double-ended queue and declaraion of node
 *         data structure. 
 *
 *  @author Ke Wu (kewu)
 *
 *  @bug No known bugs.
 */

#ifndef _QUEUE_H_
#define _QUEUE_H_

/** @brief The node strcuture of double-ended queue. ktid and reject are data
 *  data field, next and prev indicate the next and previous node in queue. */
typedef struct node{
    int ktid;
    int reject;
    struct node *next;
    struct node *prev;
} node_t;

/** @brief The strucure of a double-ended queue. It contains a head node and
 *         a tail node. */
typedef struct deque{
    node_t *head;
    node_t *tail;
} deque_t;

int queue_init(deque_t *deque);

void enqueue(deque_t *deque, node_t* element);

node_t* dequeue(deque_t *deque);

int queue_destroy(deque_t *deque);

int queue_is_active(deque_t *deque);

#endif
