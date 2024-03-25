#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>

typedef struct args {
    int* n;
    sem_t* mutex;
} args;

void* add (void* arg) {
    args* a = (args*) arg;
    int* n = a->n;
    sem_t* mutex = a->mutex;

    for (int i=0 ; i<100000 ; i++) {
        sem_wait(mutex);
        (*n)++;
        sem_post(mutex);
    }
}

void* subtract (void* arg) {
    struct args* a = (struct args*) arg;
    int* n = a->n;
    sem_t* mutex = a->mutex;

    for (int i=0 ; i<100000 ; i++) {
        sem_wait(mutex);
        (*n)--;
        sem_post(mutex);
    }
}

void main () {
    int n = 0;
    sem_t mutex;
    sem_init(&mutex, 0, 1);

    args arg = {&n, &mutex};

    pthread_t p1, p2;
    pthread_create(&p1, NULL, add, &arg);
    pthread_create(&p2, NULL, subtract, &arg);

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);

    printf("Final value of n: %d (expected 0)\n", n);
}