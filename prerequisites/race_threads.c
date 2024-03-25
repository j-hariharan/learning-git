#include<stdio.h>
#include<pthread.h>


void* add (void* arg) {
    int* n = (int*) arg;

    for (int i=0 ; i<100000 ; i++) {
        (*n)++;
    }
}

void* subtract (void* arg) {
    int* n = (int*) arg;

    for (int i=0 ; i<100000 ; i++) {
        (*n)--;
    }
}

void main () {
    int n = 0;

    pthread_t p1, p2;
    pthread_create(&p1, NULL, add, &n);
    pthread_create(&p2, NULL, subtract, &n);

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);

    printf("Final value of n: %d (expected 0)\n", n);
}