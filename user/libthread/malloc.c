/*
 * these functions should be thread safe.
 * It is up to you to rewrite them
 * to make them thread safe.
 *
 */

#include <stdlib.h>
#include <types.h>
#include <stddef.h>

#include <mutex.h>
#include <lib_public.h>

/** @brief Mutex to guard malloc library */
mutex_t mutex_malloc;

int malloc_init() {
    mutex_init(&mutex_malloc);
    return 0;
}

void *malloc(size_t __size)
{
    mutex_lock(&mutex_malloc);
    void *ret = _malloc(__size);
    mutex_unlock(&mutex_malloc);

    return ret;
}

void *calloc(size_t __nelt, size_t __eltsize)
{
    mutex_lock(&mutex_malloc);
    void *ret = _calloc(__nelt, __eltsize);
    mutex_unlock(&mutex_malloc);

    return ret;
}

void *realloc(void *__buf, size_t __new_size)
{
    mutex_lock(&mutex_malloc);
    void *ret = _realloc(__buf, __new_size);
    mutex_unlock(&mutex_malloc);

    return ret;
}

void free(void *__buf)
{
    mutex_lock(&mutex_malloc);
    _free(__buf);
    mutex_unlock(&mutex_malloc);
}

