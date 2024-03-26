#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>

#include "../helpers/random_matrix.h"
#include "../helpers/determinant.h"
#include "../helpers/timer.h"

#define MAX_TIMES 3


typedef struct args {
    int n;
    int* result;
    int* M;
    int coeff;
    sem_t* mutex;
    int times;
} args;


void* determinant(void* a) {
    args* a_ = (args*) a;
    int n = a_->n;
    int* result = a_->result;
    int* M = a_->M;
    int coeff = a_->coeff;
    sem_t* mutex = a_->mutex;
    int times = a_->times;
    
    // base case
    if (n==2) {
        // update shared result
        sem_wait(mutex);
            // critical section
            *result += coeff * (M[0]*M[3] - M[1]*M[2]);
        sem_post(mutex);
    
        // free up dynamically allocated memory
        free(M);
        free(a);
        return NULL;
    }

    if (times == 0) {
        int M1[n][n];
        for (int i=0 ; i<n ; i++) {
            for (int j=0 ; j<n ; j++) {
                M1[i][j] = M[i*n + j];
            }
        }
        int res = naive_determinant(n, M1);

        // update shared result
        sem_wait(mutex);
            // critical section
            *result += coeff * res;
        sem_post(mutex);

        // free up dynamically allocated memory
        free(M);
        free(a);
        return NULL;
    }

    // create and init new shared variable and mutex
    int newres = 0;
    sem_t newmutex;
    sem_init(&newmutex, 0, 1);

    // array to store thread IDs
    pthread_t threads[n];

    
    for (int j=0 ; j<n ; j++) {
        // find cofactor of every element in first row

        // matrix made from removing row and column
        int* M1 = (int*) malloc(sizeof(int)*(n-1)*(n-1));
        for (int i=1 ; i<n ; i++) {
            int count = 0;
            for (int k=0 ; k<n ; k++) {
                if (k == j) continue;
                M1[(i-1)*(n-1)+count] = M[i*n+k];
                count++;
            }
        }


        // create new args for child thread
        args* a1 = (args*) malloc(sizeof(args));
        a1->result = &newres;
        a1->M = M1;
        a1->n = n-1;
        a1->coeff = M[j];
        if (j%2 != 0) a1->coeff *= -1;
        a1->mutex = &newmutex;
        a1->times = times-1;

        // create child thread
        pthread_create(&threads[j], NULL, determinant, a1);
    }

    // wait for all threads to complete execution
    for (int j=0 ; j<n ; j++) {
        pthread_join(threads[j], NULL);
    }

    // update shared result
    sem_wait(mutex);
        // critical section
        *result += coeff * newres;
    sem_post(mutex);
    
    // free up dynamically allocated memory
    free(M);
    free(a);
}

int determinant_single(int n, int M[n][n]) {
    // base case
    if (n==2) {
        return M[0][0]*M[1][1] - M[0][1]*M[1][0];
    }

    int sum = 0;

    
    for (int j=0 ; j<n ; j++) {
        // find cofactor of every element in first row

        // matrix made from removing row and column
        int M1[n-1][n-1];
        for (int i=1 ; i<n ; i++) {
            int count = 0;
            for (int k=0 ; k<n ; k++) {
                if (k == j) continue;
                M1[i-1][count] = M[i][k];
                count++;
            }
        }

        // recursively calculate determinant of smaller matrix
        int res = determinant_single(n-1, M1);
        if (j%2 != 0) res *= -1;

        // add to sum
        sum += res * M[0][j];
    }

    return sum;
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

    int* M1 = (int*) malloc(sizeof(int)*n*n);
    for (int i=0 ; i<n ; i++) {
        for (int j=0 ; j<n ; j++) {
            M1[i*n + j] = M[i][j];
        }
    }


    int result;
    args* a = (args*) malloc(sizeof(args));
    sem_t mutex;
    sem_init(&mutex, 0, 1);

    a->n = n;
    a->result = &result;
    a->M = M1;
    a->coeff = 1;
    a->mutex = &mutex;
    a->times = MAX_TIMES;


    int t = start();
    determinant(a);
    int ms = end(t);

    printf("\nDeterminant: %d (%dms)\n", result, ms);
    
    t = start();
    result = naive_determinant(n, M);
    ms = end(t);

    printf("Naive determinant: %d (%dms)\n\n", result, ms);
}