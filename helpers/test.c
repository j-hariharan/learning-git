#include<stdio.h>

#include "random_matrix.h"
#include "determinant.h"
#include "timer.h"

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

    int t = start();
    int res = naive_determinant(n, M);
    int ms = end(t);
    
    printf("\nDeterminant: %d (%dms)\n\n", res, ms);
}