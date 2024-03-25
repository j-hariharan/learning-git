#include<stdio.h>
#include<pthread.h>

void* sayHello (void* arg) {
    printf("From thread!\n");
}

void main () {
    pthread_t p1;

    pthread_create(&p1, NULL, sayHello, NULL);
    printf("From parent process!\n");
    pthread_join(p1, NULL);

    printf("Joined the thread!\n\n");
}