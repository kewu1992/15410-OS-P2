/** @file queue.c
 *
 *  @brief This file contains the implementation of a double-ended queue
 */

#include <stdlib.h>
#include <queue.h>

int queue_init(deque_t *deque){
    deque->head = malloc(sizeof(node_t));
    deque->tail = malloc(sizeof(node_t));
    if (!deque->head || !deque->tail)
        return -1;
    deque->head->next = deque->tail;
    deque->head->prev = NULL;
    deque->tail->prev = deque->head;
    deque->tail->next = NULL;
    return 0;
}

void enqueue(deque_t *deque, node_t* element) {
    element->prev = deque->tail->prev;
    element->next = deque->tail;
    deque->tail->prev->next = element;
    deque->tail->prev = element;
}

node_t* dequeue(deque_t *deque) {
    if (deque->head->next == deque->tail)
        return NULL;
    node_t* element = deque->head->next;
    deque->head->next = deque->head->next->next;
    deque->head->next->prev = deque->head;
    return element;
}

int queue_destroy(deque_t *deque) {
    if (deque->head->next != deque->tail)
        return -1;
    free(deque->head);
    free(deque->tail);
    deque->head = NULL;
    deque->tail = NULL;
    return 0;
}

int queue_is_active(deque_t *deque) {
    return (!deque->head || !deque->tail) ? 0 : 1;
}
