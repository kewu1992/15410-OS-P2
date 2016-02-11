#include <arraylist.h>
#include <stdlib.h>


arraylist_t* arraylist_init(int size) {
    if (size <= 0)
        return NULL;
    arraylist_t *array = malloc(sizeof(arraylist_t));
    array->maxsize = size;
    array->cursize = 0;
    array->data = calloc(size, sizeof(int));
    return array;
}

static void double_array(arraylist_t* array) {
    int newsize = array->maxsize * 2;
    int* newdata = calloc(newsize, sizeof(int));
    
    int i;
    for (i = 0; i < array->cursize; i++)
        newdata[i] = array->data[i];

    array->maxsize = newsize;
    free(array->data);
    array->data = newdata;
}

int arraylist_insert(arraylist_t* array, int value) {
    if (value == 0)
        return -1;
    int i;
    for (i = 0; i < array->cursize; i++)
        if (array->data[i] == 0) {
            array->data[i] = value;
            return i;
        }

    if (array->cursize == array->maxsize)
        double_array(array);

    array->data[array->cursize] = value;
    return array->cursize++;
}

int arraylist_remove(arraylist_t* array, int value) {
    if (value == 0)
        return -1;
    int i;
    for (i = 0; i < array->cursize; i++)
        if (array->data[i] == value) {
            array->data[i] = 0;
            return 0;
        }
    return -1;
}

int arraylist_get(arraylist_t* array, int index) {
    if (index >= array->cursize)
        return 0;
    else
        return array->data[index];
}

int arraylist_set(arraylist_t* array, int index, int value) {
    if (index >= array->cursize)
        return -1;
    array->data[index] = value;
    return 0;
}

int arraylist_find(arraylist_t* array, int value) {
    if (value == 0)
        return -1;
    int i;
    for (i = 0; i < array->cursize; i++)
       if (array->data[i] == value)
            return i;
    return -1; 
}

void arraylist_free(arraylist_t* array) {
    free(array->data);
    free(array);
}