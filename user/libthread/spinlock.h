/** @file spinlock.h
 *
 *  @brief Constains the macro implementation of spinlock
 *  
 *  @author Jian Wang (jianwan3)
 *  @author Ke Wu (kewu)
 *  @bug No known bugs
 */
#ifndef _SPINLOCK_H
#define _SPINLOCK_H

#include <thr_internals.h>
#include <syscall.h>

/**@ brief spinlock type */
typedef int spinlock_t;

/**@ brief Maximal number of spinning times trying to acquire a lock */
#define MAX_SPIN_NUM  1000

/**@ brief Initialize spin lock */
#define SPINLOCK_INIT(lock)     *(lock) = 1
/**@ brief Destory spin lock */
#define SPINLOCK_DESTROY(lock)  asm_xchg(lock, 0)
/**@ brief Lock spin lock */
#define SPINLOCK_LOCK(lock)     do { \
    while (1) { \
        int i = 0; \
        while (i<MAX_SPIN_NUM &&   \
                !asm_xchg(lock, 0)) \
        i++; \
        if (i==MAX_SPIN_NUM) \
        yield(-1); \
        else \
        break; \
    } \
} while(0)
/**@ brief Unlock spin lock */
#define SPINLOCK_UNLOCK(lock)   asm_xchg(lock, 1)

#endif /* _SPINLOCK_H */

