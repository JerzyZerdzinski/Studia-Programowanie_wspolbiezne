#ifndef LCL_LIST_H
#define LCL_LIST_H

#include <pthread.h>
// ==============================================
//
//  Version 1.1, 2025-01-26
//
// ==============================================

struct TList {
	void **items;         
    int maxSize;    
	int nextSize;      
    int currentSize;      
    int first;            
    int last;             
    pthread_mutex_t lock; 
    pthread_cond_t notFull;  // lista niepełna
    pthread_cond_t notEmpty; // lista niepusta
};
typedef struct TList TList;

TList* createList(int s);

void destroyList(TList* lst);

void putItem(TList* lst, void* itm);

void* getItem(TList* lst);

void* popItem(TList* lst);

int removeItem(TList* lst, void* itm);

int getCount(TList* lst);

void setMaxSize(TList* lst, int s);

void appendItems(TList* lst, TList* lst2);

void showList(TList* lst);

#endif //LCL_LIST_H