#include <mutex.h>
#include <thread.h>
#include <syscall.h>
#include <cond_type.h>
#include <stdlib.h>
#include <queue.h>
#include <thr_internals.h>

int cond_init(cond_t *cv) {
    mutex_init(&cv->mutex);
    queue_init(&cv->deque);
    return 0;
}

void cond_destroy(cond_t *cv) {
    mutex_destroy(&cv->mutex);
    if (queue_destroy(&cv->deque) < 0){
        // illegal, some threads are blocked waiting on it
    }
}

void cond_wait(cond_t *cv, mutex_t *mp) {
    node_t *tmp = malloc(sizeof(node_t));
    tmp->ktid = thr_getktid();
    tmp->reject = 0;

    mutex_lock(&cv->mutex);

    enqueue(&cv->deque, tmp);

    mutex_unlock(mp);

    mutex_unlock(&cv->mutex);
    deschedule(&tmp->reject);

    free(tmp);

    mutex_lock(mp);
}

void cond_signal(cond_t *cv) {
    mutex_lock(&cv->mutex);
    node_t *tmp = dequeue(&cv->deque);
    if (tmp) {
        int tmp_ktid = tmp->ktid;
        tmp->reject = 1;
        make_runnable(tmp_ktid);
    }
    mutex_unlock(&cv->mutex);
}

void cond_broadcast(cond_t *cv) {
    mutex_lock(&cv->mutex);
    node_t *tmp = dequeue(&cv->deque);
    while (tmp) {
        int tmp_ktid = tmp->ktid;
        tmp->reject = 1;
        make_runnable(tmp_ktid);
        tmp = dequeue(&cv->deque);
    }
    mutex_unlock(&cv->mutex);
}
