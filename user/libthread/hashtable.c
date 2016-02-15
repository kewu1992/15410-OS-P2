#include <stdlib.h>
#include <hashtable.h>

int hashtable_init(hashtable_t *table) {
    table->array = malloc(sizeof(hashnode_t) * table->size);
    int i;
    for (i = 0; i < table->size; i++)
        table->array[i].next = NULL;

    return mutex_init(&table->lock);
}

void hashtable_put(hashtable_t *table, void* key, void* value) {
    int index = table->func(key);

    hashnode_t *hp = malloc(sizeof(hashnode_t));
    hp->key = key;
    hp->value = value;

    mutex_lock(&table->lock);
    hp->next = table->array[index].next;
    table->array[index].next = hp;
    mutex_unlock(&table->lock);
}

void* hashtable_get(hashtable_t *table, void* key, int *is_find) {
    int index = table->func(key);

    mutex_lock(&table->lock);
    hashnode_t *hp = &(table->array[index]);
    while (hp->next) {
        if (hp->next->key == key) {
            void* rv = hp->next->value;
            mutex_unlock(&table->lock);
            *is_find = 1;
            return rv;
        }
        hp = hp->next;
    }
    mutex_unlock(&table->lock);

    *is_find = 0;
    return NULL;
} 

void* hashtable_remove(hashtable_t *table, void* key, int *is_find) {
    int index = table->func(key);

    mutex_lock(&table->lock);
    hashnode_t *hp = &(table->array[index]);
    while (hp->next) {
        if (hp->next->key == key) {
            hashnode_t *tmp = hp->next;
            hp->next = hp->next->next;
            mutex_unlock(&table->lock);

            void *rv = tmp->value;
            free(tmp);
            *is_find = 1;
            return rv;
        }
        hp = hp->next;
    }
    mutex_unlock(&table->lock);
    
    *is_find = 0;
    return NULL;
} 

void hashtable_destroy(hashtable_t *table) {
    hashnode_t *hp;
    int i;
    for (i = 0; i < table->size; i++){
        hp = &(table->array[i]);
        while (hp->next) {
            hashnode_t *tmp = hp->next;
            hp->next = hp->next->next;
            free(tmp);
        }
    }
    free(table->array);
}