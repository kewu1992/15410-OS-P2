#include <rwlock.h>

int rwlock_init( rwlock_t *rwlock ) { return 0;}
void rwlock_lock( rwlock_t *rwlock, int type ) {}
void rwlock_unlock( rwlock_t *rwlock ) {}
void rwlock_destroy( rwlock_t *rwlock ) {}
void rwlock_downgrade( rwlock_t *rwlock) {}

