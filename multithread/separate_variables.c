#include<stdio.h>
#include<pthread.h>

#include "../helpers/random_matrix.h"
#include "../helpers/determinant.h"
#include "../helpers/timer.h"

#define MAX_TIMES 3



typedef struct args {
    int n;
    int* result;
    int* M;
    int times;
} args;


void* determinant(void* a) {
    args* a_ = (args*)a;
    int n = a_->n;
    int* result = a_->result;
    int* M = a_->M;
    int times = a_->times;
    
    // base case
    if (n==2) {
        *result = M[0]*M[3] - M[1]*M[2];

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
        *result = naive_determinant(n, M1);

        // free up dynamically allocated memory
        free(M);
        free(a);
        return NULL;
    }


    // array to store result of each smaller matrix
    int results[n];
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
        a1->result = &results[j];
        a1->M = M1;
        a1->n = n-1;
        a1->times = times-1;

        // create child thread
        pthread_create(&threads[j], NULL, determinant, a1);
    }

    // join threads and calculate final determinant
    *result = 0;
    for (int j=0 ; j<n ; j++) {
        pthread_join(threads[j], NULL);
        if (j%2 != 0) {
            results[j] *= -1;
        }
        *result += M[j] * results[j];
    }

    // free up dynamically allocated memory
    free(M);
    free(a);
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
    a->n = n;
    a->result = &result;
    a->M = M1;
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