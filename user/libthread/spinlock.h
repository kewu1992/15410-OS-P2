#ifndef _SPINLOCK_H
#define _SPINLOCK_H

#include <thr_internals.h>
#include <syscall.h>

typedef int spinlock_t;

#define MAX_SPIN_NUM  1000

#define SPINLOCK_INIT(lock)     *(lock) = 1
#define SPINLOCK_DESTROY(lock)  asm_xchg(lock, 0)
#define SPINLOCK_LOCK(lock)     do { \
                                    while (1) { \
                                        int i = 0; \
                                        while (i<MAX_SPIN_NUM && !asm_xchg(lock, 0)) \
                                            i++; \
                                        if (i==MAX_SPIN_NUM) \
                                            yield(-1); \
                                        else \
                                            break; \
                                    } \
                                } while(0)
#define SPINLOCK_UNLOCK(lock)   asm_xchg(lock, 1)

#endif /* _SPINLOCK_H */
