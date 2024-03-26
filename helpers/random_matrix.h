#include<stdlib.h>

void generate_matrix (int n, int M[n][n], int seed) {
    srand(seed);

    for (int i = 0; i < n; ++i) 
        for (int j = 0; j < n; ++j) 
            M[i][j] = rand() % 51;
}
