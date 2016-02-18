#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#include <mutex.h>

typedef struct hashnode_s{
    struct hashnode_s *next;
    void *key;
    void *value;
} hashnode_t;

typedef struct {
    hashnode_t *array;
    int size;
    int (*func)(void *);
    mutex_t lock;
} hashtable_t;

int hashtable_init(hashtable_t *table);

int hashtable_put(hashtable_t *table, void* key, void* vaule);

void* hashtable_get(hashtable_t *table, void* key, int *is_find);

void* hashtable_remove(hashtable_t *table, void* key, int *is_find);

void hashtable_destroy(hashtable_t *table);

#endif