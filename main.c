#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "list.h"

void* producer(void* arg) {
    TList* list = (TList*)arg;

    for (int i = 0; i < 100; i++) { 
        int* item = (int*)malloc(sizeof(int));
        *item = i;
        printf("Producent: próba dodania elementu %d\n", *item);
        putItem(list, item);
        printf("Producent: dodano element %d\n", *item);
        showList(list);
    }

    return NULL;
}

void* consumer(void* arg) {
    TList* list = (TList*)arg;

    sleep(2); 
    for (int i = 0; i < 100; i++) {
        int* item = (int*)getItem(list);
        printf("Konsument: zabrano element %d\n", *item);
        free(item); 
        usleep(100000); 
        showList(list);
    }

    return NULL;
}

void* resizer(void* arg) {
    TList* list = (TList*)arg;

    sleep(2); 

    printf("Resizer: Zwiększenie maksymalnego rozmiaru listy do 10\n");
    setMaxSize(list, 10);
    showList(list);
    sleep(1);
    printf("Resizer: Zmniejszenie maksymalnego rozmiaru listy do 3\n");
    setMaxSize(list, 3);
    showList(list);

    return NULL;
}

void testProducerConsumerWithResize() {
    printf("Test: Zmiana rozmiaru tablicy podczas pracy producenta i konsumenta...\n");

    TList* list = createList(5); 

    pthread_t producerThread, consumerThread, resizerThread;

    pthread_create(&producerThread, NULL, producer, list);
    pthread_create(&consumerThread, NULL, consumer, list);
    pthread_create(&resizerThread, NULL, resizer, list);

    pthread_join(producerThread, NULL);
    pthread_join(consumerThread, NULL);
    pthread_join(resizerThread, NULL);

    printf("Końcowy stan listy:\n");
    showList(list);

    destroyList(list);
}



void testProducerConsumerWait() {
    printf("Test: Producent czeka, aż konsument zabierze element...\n");

    TList* list = createList(5); 

    pthread_t producerThread, consumerThread;

    pthread_create(&producerThread, NULL, producer, list);
    pthread_create(&consumerThread, NULL, consumer, list);

    pthread_join(producerThread, NULL);
    pthread_join(consumerThread, NULL);

    printf("Końcowy stan listy:\n");
    showList(list);

    destroyList(list);
}

void testAppendItems() {
    printf("Test: Łączenie dwóch list za pomocą appendItems...\n");

    // Tworzenie dwóch list
    TList* list1 = createList(5);
    TList* list2 = createList(5);

    // Dodawanie elementów do listy 1
    for (int i = 0; i < 5; i++) {
        int* item = (int*)malloc(sizeof(int));
        *item = i + 1; // Elementy: 1, 2, 3, 4, 5
        putItem(list1, item);
    }

    printf("Lista 1 przed appendItems:\n");
    showList(list1);

    // Dodawanie elementów do listy 2
    for (int i = 5; i < 8; i++) {
        int* item = (int*)malloc(sizeof(int));
        *item = i + 1; // Elementy: 6, 7, 8
        putItem(list2, item);
    }

    printf("Lista 2 przed appendItems:\n");
    showList(list2);

    // Łączenie list
    appendItems(list1, list2);

    printf("Lista 1 po appendItems:\n");
    showList(list1);

    printf("Lista 2 po appendItems\n");
    showList(list2);
}

int main() {
    testProducerConsumerWait();
    testProducerConsumerWithResize();
    testAppendItems();
    return 0;
}