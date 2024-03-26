// calculate the determinant of a matrix
// using multiprocessing
// using shared memory for inter-process communication

#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<semaphore.h>

#include "../helpers/random_matrix.h"
#include "../helpers/determinant.h"
#include "../helpers/timer.h"

#define MAX_TIMES 3



int determinant (int n, int M[n][n], int times, int* shmcount, sem_t* shmcountmutex) {
    // base case
    if (n == 2) {
        return M[0][0] * M[1][1] - M[0][1] * M[1][0];
    }

    if (times == 0) {
        return naive_determinant(n, M);
    }

    // create new shared variables and increment shared memory count
    // critical section
    sem_wait(shmcountmutex);

        // creating shared memory
        int shmid = shmget((key_t) (*shmcount)+1, sizeof(int), 0666 | IPC_CREAT);
        int* result = (int*) shmat(shmid, (void*)0, 0);
        (*result) = 0;

        // create shared semaphore
        int semshmid = shmget((key_t) (*shmcount)+2, sizeof(sem_t), 0666 | IPC_CREAT);
        sem_t* mutex = (sem_t*) shmat(semshmid, (void*)0, 0);
        sem_init(mutex, 1, 1);

        // increment shared memory count
        (*shmcount) += 2;

    sem_post(shmcountmutex);
    
    // creating child processes
    for (int j = 0; j < n; j++) {
        int pid = fork();

        if (pid == 0) {
            int M1[n - 1][n - 1];

            // matrix made from removing row and column
            for (int i = 1; i < n; i++) {
                int count = 0;
                for (int k = 0; k < n; k++) {
                    if (k == j)
                        continue;
                    M1[i - 1][count++] = M[i][k];
                }
            }

            // recursively calculate determinant of smaller matrix
            int res = determinant(n - 1, M1, times-1, shmcount, shmcountmutex);
            if (j%2 != 0) res *= -1;

            // add cofactor to shared result variable
            // critical section
            sem_wait(mutex);
                (*result) += res * M[0][j];
            sem_post(mutex);
            _exit(0);
        }
    }

    // wait for all n child processes to finish executing
    for (int j = 0; j < n; j++) {
        wait(NULL);
    }

    int res = *result;

    // detatch and destroy shared memory
    shmdt(result);
    shmctl(shmid, IPC_RMID, NULL);

    sem_destroy(mutex);
    shmdt(mutex);
    shmctl(semshmid, IPC_RMID, NULL);

    return res;
}


void main (int argc, char* argv[]) {
    int n, seed = 0;

    if (argc == 3) {
        n = atoi(argv[1]);
        seed = atoi(argv[2]);
    } else if (argc == 2) {
        n = atoi(argv[1]);
    } else {
        printf("Enter size of the array: ");
        scanf("%d", &n);
    }

    int M[n][n];
    generate_matrix(n, M, seed);


    // creating shared memory
    // to keep count of number of shared variables used
    int shmcountid = shmget((key_t) 1, sizeof(int), 0666 | IPC_CREAT);
    int* shmcount = (int*) shmat(shmcountid, (void*)0, 0);
    (*shmcount) = 2;

    // mutex for shared memory count
    int shmcountmutexid = shmget((key_t) 2, sizeof(sem_t), 0666 | IPC_CREAT);
    sem_t* shmcountmutex = (sem_t*) shmat(shmcountmutexid, (void*)0, 0);
    sem_init(shmcountmutex, 1, 1);



    int t = start();
    int res = determinant(n, M, MAX_TIMES, shmcount, shmcountmutex);
    int ms = end(t);

    printf("\nDeterminant: %d (%dms)\n", res, ms);
    
    t = start();
    res = naive_determinant(n, M);
    ms = end(t);

    printf("Naive determinant: %d (%dms)\n\n", res, ms);


    // detatch and destroy shared memory
    shmdt(shmcount);
    shmctl(shmcountid, IPC_RMID, NULL);

    sem_destroy(shmcountmutex);
    shmdt(shmcountmutex);
    shmctl(shmcountmutexid, IPC_RMID, NULL);
}