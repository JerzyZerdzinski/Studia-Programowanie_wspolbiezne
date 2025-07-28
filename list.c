#include "list.h"
#include <stdlib.h>
#include <stdio.h>
 
TList* createList(int size) {
    TList *list = (TList *)malloc(sizeof(TList));
    list->items = (void **)malloc(size * sizeof(void *));
    list->maxSize = size;
    list->currentSize = 0;
    list->first = 0;
    list->last = 0;
    pthread_mutex_init(&list->lock, NULL);
    pthread_cond_init(&list->notFull, NULL);
    pthread_cond_init(&list->notEmpty, NULL);
    return list;
}

void putItem(TList *list, void *el) {
    pthread_mutex_lock(&list->lock);
    while (list->currentSize >= list->maxSize) {
        pthread_cond_wait(&list->notFull, &list->lock); 
    }

    list->items[list->last] = el;
    list->last = (list->last + 1) % list->maxSize;
    list->currentSize++;

    pthread_cond_signal(&list->notEmpty); 
    pthread_mutex_unlock(&list->lock);
}

void* getItem(TList *list) {
    pthread_mutex_lock(&list->lock);

    while (list->currentSize == 0) {
        pthread_cond_wait(&list->notEmpty, &list->lock); 
    }
        void *item = list->items[list->first];
    if (list->currentSize<=list->maxSize){
        list->first = (list->first + 1) % list->maxSize;
        list->currentSize--;
    }
    else{
        int index = 0;
        for (int i = 0; i < list->currentSize - 1; i++) {
            int nextIndex = (index + 1) % list->currentSize;
            list->items[index] = list->items[nextIndex];
            index = nextIndex;
        }
        list->currentSize--;
    }

    pthread_cond_signal(&list->notFull); 
    pthread_mutex_unlock(&list->lock);

    return item;
}

void* popItem(TList *list) {
    pthread_mutex_lock(&list->lock); 

    while (list->currentSize == 0) {
        pthread_cond_wait(&list->notEmpty, &list->lock); 
    }

    void *item = list->items[(list->last - 1 + list->maxSize) % list->maxSize];
    list->last = (list->last - 1 + list->maxSize) % list->maxSize;
    list->currentSize--; 

    pthread_cond_signal(&list->notFull);
    pthread_mutex_unlock(&list->lock); 
    return item; 
}

int removeItem(TList *list, void *el) {
    pthread_mutex_lock(&list->lock); 
    int index = list->first; 
    int found = -1; 

    for (int i = 0; i < list->currentSize; i++) {
        if (list->items[index] == el) { 
            found = index;
            break;
        }
        index = (index + 1) % list->maxSize; 
    }

    if (found == -1) {
        pthread_mutex_unlock(&list->lock); 
        return 0; 
    }

    index = found;
    for (int i = 0; i < list->currentSize - 1; i++) {
        int nextIndex = (index + 1) % list->maxSize;
        list->items[index] = list->items[nextIndex];
        index = nextIndex;
    }

    list->last = (list->last - 1 + list->maxSize) % list->maxSize;
    list->currentSize--;
    pthread_cond_signal(&list->notFull);
    pthread_mutex_unlock(&list->lock); 
    return 1; 
}

int getCount(TList *list) {
    pthread_mutex_lock(&list->lock); 
    int count = list->currentSize;  
    pthread_mutex_unlock(&list->lock); 
    return count; 
}

void setMaxSize(TList *list, int n) {
    pthread_mutex_lock(&list->lock); 
    list->nextSize=n;
    if (n > list->maxSize) {
        void **newItems = (void **)malloc(n * sizeof(void *));
        for (int i = 0; i < list->currentSize; i++) {
            newItems[i] = list->items[(list->first + i) % list->maxSize];
        }
        free(list->items); 
        list->items = newItems; 
        list->first = 0; 
        list->last = list->currentSize; 
        list->maxSize = n; 
    }
    else if (n < list->maxSize) {
        void **newItems = (void **)malloc(list->currentSize * sizeof(void *));
        for (int i = 0; i < list->currentSize; i++) {
            newItems[i] = list->items[(list->first + i) % list->maxSize];
        }
        free(list->items); 
        list->items = newItems; 
        list->first = 0; 
        list->last = 0; 
        list->maxSize = n; 
    }
    pthread_mutex_unlock(&list->lock);
}

void appendItems(TList *list, TList *list2) {
    pthread_mutex_lock(&list->lock);   
    pthread_mutex_lock(&list2->lock); 
    int dif = list->currentSize + list2->currentSize - list->maxSize;
    if (dif > 0) {
        void **newItems = (void **)malloc((list->currentSize + list2->currentSize) * sizeof(void *));
        for (int i = 0; i < list->currentSize; i++) {
            newItems[i] = list->items[(list->first + i) % list->maxSize];
        }

        free(list->items); 
        list->items = newItems; 
        list->first = 0; 
        list->last = list->currentSize; 
        list->maxSize = list->currentSize + list2->currentSize; 
    }
    while (list2->currentSize > 0) {
        void *item = list2->items[list2->first];
        list2->first = (list2->first + 1) % list2->maxSize;
        list2->currentSize--;

        list->items[list->last] = item;
        list->last = (list->last + 1) % list->maxSize;
        list->currentSize++;
    }
    pthread_cond_signal(&list2->notFull); 
    pthread_mutex_unlock(&list2->lock); 
    pthread_mutex_unlock(&list->lock);  
}

void destroyList(TList *list) {
    pthread_mutex_lock(&list->lock);
    for (int i = 0; i < list->currentSize; i++) {
        void *item = list->items[(list->first + i) % list->maxSize];
        free(item); 
    }
    free(list->items);
    pthread_mutex_unlock(&list->lock); 
    pthread_mutex_destroy(&list->lock);
    pthread_cond_destroy(&list->notFull);
    pthread_cond_destroy(&list->notEmpty);
    free(list);
}

void showList(TList *list) {
    pthread_mutex_lock(&list->lock);
    // printf("pierwszy: %d\n",list->first);
    // printf("ostatni: %d\n", list->last);
    if (list->currentSize <= list->maxSize){
        for (int i = 0; i < list->currentSize; i++) {
            int index = (list->first + i) % list->maxSize;
            printf("%d", *((int*)list->items[index]));
            if (i < list->currentSize - 1) {
                printf(", ");
            }
        }
    }
    else {
        for (int i = 0; i < list->currentSize; i++) {
            int index = (list->first + i) % list->currentSize;
            printf("%d", *((int*)list->items[index]));
            if (i < list->currentSize - 1) {
                printf(", ");
            }
        }
    }
    printf("\n");
    pthread_mutex_unlock(&list->lock); 
}