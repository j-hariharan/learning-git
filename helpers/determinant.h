int naive_determinant(int n, int M[n][n]) {
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
        int res = naive_determinant(n-1, M1);
        if (j%2 != 0) res *= -1;

        // add to sum
        sum += res * M[0][j];
    }

    return sum;
}