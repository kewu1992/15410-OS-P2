/*
 * these functions should be thread safe.
 * It is up to you to rewrite them
 * to make them thread safe.
 *
 */

#include <stdlib.h>
#include <types.h>
#include <stddef.h>

#include <spinlock.h>

/** @brief Mutex to guard malloc library */
spinlock_t mutex_malloc;

int malloc_init() {
    SPINLOCK_INIT(&mutex_malloc);
    return 0;
}

void *malloc(size_t __size)
{
    SPINLOCK_LOCK(&mutex_malloc);
    void *ret = _malloc(__size);
    SPINLOCK_UNLOCK(&mutex_malloc);

    return ret;
}

void *calloc(size_t __nelt, size_t __eltsize)
{
    SPINLOCK_LOCK(&mutex_malloc);
    void *ret = _calloc(__nelt, __eltsize);
    SPINLOCK_UNLOCK(&mutex_malloc);

    return ret;
}

void *realloc(void *__buf, size_t __new_size)
{
    SPINLOCK_LOCK(&mutex_malloc);
    void *ret = _realloc(__buf, __new_size);
    SPINLOCK_UNLOCK(&mutex_malloc);

    return ret;
}

void free(void *__buf)
{
    SPINLOCK_LOCK(&mutex_malloc);
    _free(__buf);
    SPINLOCK_UNLOCK(&mutex_malloc);
}

