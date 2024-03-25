#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/wait.h>

int main () {
    int shmid = shmget((key_t) 1, sizeof(int), 0666 | IPC_CREAT);
    int* n = (int*) shmat(shmid, (void*)0, 0);
    *n = 100;

    pid_t pid = fork();

    if (pid == 0) {
        printf("Child process\n");
        printf("Value of n = %d... updating to 200\n", *n);
        *n = 200;
    } else {
        printf("Parent process\nWaiting for child process to complete execution\n");
        wait(NULL);
        printf("Value of n from parent process = %d\n", *n);

        shmdt(n);
        shmctl(shmid, IPC_RMID, NULL);
    }
}
