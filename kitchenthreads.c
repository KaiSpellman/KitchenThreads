#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_COOKS 3
#define NUM_WAITERS 3
#define MAX_DISHES 5

sem_t empty_slots;     // Semaphore to count empty dish slots
sem_t full_slots;      // Semaphore to count full dish slots 
pthread_mutex_t mutex; // Mutex to protect shared buffer

int dishes = 0; // Shared dish counter

void* cook(void* arg) {
    int id = *(int*)arg;
    while (1) {
        sleep(rand() % 3); // Simulate cooking time

        sem_wait(&empty_slots); // Wait if dish buffer is full
        pthread_mutex_lock(&mutex);

        dishes++;
        printf("Cook %d prepared a dish. Total dishes: %d\n", id, dishes);

        pthread_mutex_unlock(&mutex);
        sem_post(&full_slots); // Signal that a dish is ready
    }
    return NULL;
}

void* waiter(void* arg) {
    int id = *(int*)arg;
    while (1) {
        sem_wait(&full_slots); // Wait for a dish to be available
        pthread_mutex_lock(&mutex);

        dishes--;
        printf("Waiter %d served a dish. Remaining dishes: %d\n", id, dishes);

        pthread_mutex_unlock(&mutex);
        sem_post(&empty_slots); // Signal that there's space for more dishes

        sleep(rand() % 3); // Simulate serving time
    }
    return NULL;
}

int main() {
    pthread_t cook_threads[NUM_COOKS];
    pthread_t waiter_threads[NUM_WAITERS];
    int ids[NUM_COOKS > NUM_WAITERS ? NUM_COOKS : NUM_WAITERS];

    sem_init(&empty_slots, 0, MAX_DISHES);
    sem_init(&full_slots, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    // Create cook threads
    for (int i = 0; i < NUM_COOKS; i++) {
        ids[i] = i + 1;
        pthread_create(&cook_threads[i], NULL, cook, &ids[i]);
    }

    // Create waiter threads
    for (int i = 0; i < NUM_WAITERS; i++) {
        ids[i] = i + 1;
        pthread_create(&waiter_threads[i], NULL, waiter, &ids[i]);
    }

    for (int i = 0; i < NUM_COOKS; i++) {
        pthread_join(cook_threads[i], NULL);
    }
    for (int i = 0; i < NUM_WAITERS; i++) {
        pthread_join(waiter_threads[i], NULL);
    }

    sem_destroy(&empty_slots);
    sem_destroy(&full_slots);
    pthread_mutex_destroy(&mutex);

    return 0;
}
