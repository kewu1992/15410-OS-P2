/** @file queue.c
 *
 *  @brief This file contains the implementation of a double-ended queue
 */

#include <stdlib.h>
#include <queue.h>

void queue_init(node_t* head, node_t* tail){
    head->next = tail;
    head->last = NULL;
    tail->last = head;
    tail->next = NULL;
}

void enqueue(node_t* head, node_t* tail, node_t* element) {
    element->last = tail->last;
    element->next = tail;
    tail->last->next = element;
    tail->last = element;
}

node_t* dequeue(node_t* head, node_t* tail) {
    if (head->next == tail)
        return NULL;
    node_t* element = head->next;
    head->next = head->next->next;
    head->next->last = head;
    return element;
}

int queue_destroy(node_t* head, node_t* tail) {
    if (head->next != tail)
        return -1;
    return 0;
}
