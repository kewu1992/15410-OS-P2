/** @file hashtable.h
 *  @brief Function prototypes of a generic hash table and declaraion of 
 *         hash table node and hash table data structure. 
 *
 *  @author Ke Wu (kewu)
 *
 *  @bug No known bugs.
 */

#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#include <mutex.h>

/** @brief The node strcuture of linked list of hash table. The hash table uses
 *         separate chaining to resolve collision. */
typedef struct hashnode_s{
    struct hashnode_s *next;
    void *key;
    void *value;
} hashnode_t;

/** @brief The hash table data structure. It contans the size and actual data
 *         of a hash table. It also contains the hash function that is used by
 *         this hash table. There is also a mutex to make sure the hash table is
 *         thread-safe. */
typedef struct {
    hashnode_t *array;
    int size;
    int (*func)(void *);
    mutex_t lock;
} hashtable_t;

int hashtable_init(hashtable_t *table);

void hashtable_put(hashtable_t *table, void* key, void* vaule);

void* hashtable_get(hashtable_t *table, void* key, int *is_find);

void* hashtable_remove(hashtable_t *table, void* key, int *is_find);

void hashtable_destroy(hashtable_t *table);

#endif