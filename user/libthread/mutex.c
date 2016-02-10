#include <mutex.h>

int asm_xchg(int *lock_available, int val);

int mutex_init(mutex_t *mp) {
    mp->lock_available = 1; 

    // In what condition will this call fail?

    // Success
    return 0;
}

void mutex_destroy(mutex_t *mp) {
    mp->lock_available = -1;
}

void mutex_lock(mutex_t *mp) {
    while(!asm_xchg(&mp->lock_available, 0)) {
        continue;
    }
}

void mutex_unlock(mutex_t *mp) {
    asm_xchg(&mp->lock_available, 1);
}

