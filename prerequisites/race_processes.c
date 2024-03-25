#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>

int main () {
    int shmid = shmget((key_t) 1, sizeof(int), 0666 | IPC_CREAT);
    int* n = (int*) shmat(shmid, (void*)0, 0);
    *n = 0;

    pid_t pid = fork();

    if (pid == 0) {
        printf("Child process\n");
        for (int i=0 ; i<100000 ; i++) {
            (*n)--;
        }
    } else {
        printf("Parent process\n");
        for (int i=0 ; i<100000 ; i++) {
            (*n)++;
        }
        wait(NULL);
        printf("Final value of n: %d (expected 0)\n", *n);
        shmdt(n);
        shmctl(shmid, IPC_RMID, NULL);
    }
}
