#ifndef _ARRAYLIST_H_
#define _ARRAYLIST_H_

typedef struct {
    int maxsize, cursize;
    int* data;
} arraylist_t;

arraylist_t* arraylist_init(int size);
int arraylist_insert(arraylist_t* array, int value);
int arraylist_remove(arraylist_t* array, int value);
int arraylist_get(arraylist_t* array, int index);
int arraylist_set(arraylist_t* array, int index, int value);
int arraylist_find(arraylist_t* array, int value);
void arraylist_free(arraylist_t* array);

#endif