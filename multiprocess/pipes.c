// calculate the determinant of a matrix
// using multiprocessing
// using pipes for inter-process communication

#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>

#include "../helpers/random_matrix.h"
#include "../helpers/determinant.h"
#include "../helpers/timer.h"

#define MAX_TIMES 3



int determinant(int n, int M[n][n], int times) {
    // base case
    if (n == 2) {
        return M[0][0] * M[1][1] - M[0][1] * M[1][0];
    }

    // max levels of recursive multiprocessing reached
    if (times == 0) {
        return naive_determinant(n, M);
    }

    // Array of process IDs
    int pids[n];
    // Array of pipes
    int fds[n][2];

    // Creating pipes and child processes
    for (int j = 0; j < n; j++)
    {
        pipe(fds[j]);
        int pid = fork();

        if (pid == 0) {
            // close read end of pipe
            close(fds[j][0]);

            // matrix made from removing row and column
            int M1[n - 1][n - 1];
            for (int i = 1; i < n; i++) {
                int count = 0;
                for (int k = 0; k < n; k++) {
                    if (k == j) continue;
                    M1[i - 1][count++] = M[i][k];
                }
            }

            // recursively calculate determinant of smaller matrix
            int res = determinant(n - 1, M1, times-1);

            // write determinant to pipe
            write(fds[j][1], &res, sizeof(res)); 

            // close write end of the pipe
            close(fds[j][1]);
            _exit(0);
        } 
        else {
            // store child process ID
            pids[j] = pid;

            // close write end of the pipe
            close(fds[j][1]);
        }
    }


    // reading determinants from pipes and compute the result
    int result = 0;
    int child_res;
    for (int j = 0; j < n; j++) {
        // wait for child process to finish execution
        waitpid(pids[j], NULL, 0);

        // read determinant from pipe
        read(fds[j][0], &child_res, sizeof(child_res));

        // close pipe
        close(fds[j][0]);

        // add cofactor to the final result
        if (j % 2 == 0) result += M[0][j] * child_res;
        else result -= M[0][j] * child_res;
    }

    return result;
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

    int t = start();
    int res = determinant(n, M, MAX_TIMES);
    int ms = end(t);
    
    printf("\nDeterminant: %d (%dms)\n", res, ms);
    
    t = start();
    res = naive_determinant(n, M);
    ms = end(t);

    printf("Naive determinant: %d (%dms)\n\n", res, ms);
}