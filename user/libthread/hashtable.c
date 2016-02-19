/** @file hashtable.c
 *  @brief This file contains implementation of a generic hash table
 *
 *  It implements some common API of a hash table, including init(), 
 *  put(), get(), remove() and delete(). The size of hash table and
 *  hash function are contained in the hashtable_t data structure 
 *  which is self-defined by user and passed to this program. The 
 *  collision resolution for this implementation is separate chaining
 *  with linked lists. This hash table is thread-safe.
 *
 *
 *  @bug array shouldn't always be extended --> watch out malloc error 
 *       (not enough memory, VM/PM)
 */

#include <stdlib.h>
#include <hashtable.h>
#include <stdio.h>
#include <simics.h>

/** @brief Initialize a hashtable data structure
 *  
 *  @param table The table that need to initialize
 *
 *  @return On success return 0, on error return a negative number      
 */
int hashtable_init(hashtable_t *table) {
    table->array = malloc(sizeof(hashnode_t) * table->size);
    if (!table->array)
        return -1;
    int i;
    for (i = 0; i < table->size; i++)
        table->array[i].next = NULL;

    return mutex_init(&table->lock);
}

/** @brief Put a <key, value> pair to a hash table
 *
 *  @param table The hash table to put <key, value> pair
 *  @param key Key of <key, value> pair
 *  @param value Value of <key, value> pair
 *
 *  @return Void
 */
void hashtable_put(hashtable_t *table, void* key, void* value) {
    int index = table->func(key);

    hashnode_t *hp = malloc(sizeof(hashnode_t));
    while (!hp) {
        lprintf("malloc failed, will try again...");
        printf("malloc failed, will try again...\n");
        yield(-1);
        hp = malloc(sizeof(hashnode_t));
    }
    hp->key = key;
    hp->value = value;

    mutex_lock(&table->lock);
    hp->next = table->array[index].next;
    table->array[index].next = hp;
    mutex_unlock(&table->lock);
}


/** @brief Given a key, return the corresponding value in a hash table
 *  
 *  This method only return the value without delete the <key, value> pair
 *  
 *  @param table The hash table to look up value of <key, value> pair
 *  @param key The key to look up value of <key, value> pair
 *  @param is_find This is also a return value, it indicates if the key is 
 *                 found in the hash table
 *
 *  @return The value of the <key, value> pair, return NULL if can not find
 *          the key in the hash table
 */
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

/** @brief Given a key, remove the corresponding <key, value> pair in a 
 *         hash table
 *  
 *  This method will return the value and also delete the <key, value> pair
 *  
 *  @param table The hash table to delete <key, value> pair
 *  @param key The key to delete <key, value> pair
 *  @param is_find This is also a return value, it indicates if the key is 
 *                 found in the hash table
 *
 *  @return The value of the <key, value> pair, return NULL if can not find
 *          the key in the hash table
 */
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

/** @brief Destroy a hashtable data structure
 *  
 *  @param table The table to be destroied
 *
 *  @return Void    
 */
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