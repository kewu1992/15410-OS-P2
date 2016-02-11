#ifndef _QUEUE_H_
#define _QUEUE_H_

typedef struct node{
    int tid;
    int reject;
    struct node *next;
    struct node *last;
} node_t;

void queue_init(node_t* head, node_t* tail);
void enqueue(node_t* head, node_t* tail, node_t* element);
node_t* dequeue(node_t* head, node_t* tail);
int queue_destroy(node_t* head, node_t* tail);

#endif