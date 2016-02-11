#include <mutex_type.h>
#include <mutex.h>
#include <thread.h>
#include <syscall.h>

int cond_init(cond_t *cv) {
    mutex_init(&cv->mutex);
    queue_init(&cv->queue_head, &cv->queue_tail);
}

void cond_destroy(cond_t *cv) {
    mutex_destroy(&cv->mutex);
    if (queue_destroy(&cv->queue_head, &cv->queue_tail) < 0){
        // illegal, some threads are blocked waiting on it
    }
}

void cond_wait(cond_t *cv, mutex_t *mp) {
    mutex_lock(&cv->mutex);

    node_t *tmp = malloc(sizeof(node_t));
    tmp->tid = thr_getid();
    tmp->reject = 0;
    enqueue(&cv->queue_head, &cv->queue_tail, tmp);

    mutex_unlock(mp);

    mutex_unlock(&cv->mutex);
    deschedule(&tmp->reject);

    mutex_lock(mp);
}

void cond_signal(cont_t* cv) {
    mutex_lock(&cv->mutex);
    node_t *tmp = dequeue(&cv->queue_head, &cv->queue_tail);
    if (tmp) {
        tmp->reject = 1;
        make_runnable(tmp->tid);
        free(tmp);
    }
    mutex_unlock(&cv->mutex);
}

void cond_broadcast(cond_t *cv) {
    mutex_lock(&cv->mutex);
    node_t *tmp = dequeue(&cv->queue_head, &cv->queue_tail);
    while (tmp) {
        tmp->reject = 1;
        make_runnable(tmp->tid);
        free(tmp);
        tmp = dequeue(&cv->queue_head, &cv->queue_tail);
    }
    mutex_unlock(&cv->mutex);
}