#ifndef _QUEUE_H_
#define _QUEUE_H_

typedef struct node{
    int ktid;
    int reject;
    struct node *next;
    struct node *prev;
} node_t;

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
