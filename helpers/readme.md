# Helpers
The following helpers were created to simplify the code while implementing the project. See [<u>`test.c`</u>](./test.c) for usage and example.

## Random Matrix Generator
[<u>`random_matrix.h`</u>](./random_matrix.h)
```C
    void generate_matrix (int n, int M[n][n], int seed)
```
The `generate_matrix` function is used to generate a matrix of a given dimension. It also takes a `seed` parameter which is used as seed to the RNG. The generated matrix is stored in the parameter `M`.


## Naive Determinant Calculator
[<u>`determinant.h`</u>](./determinant.h)
```C
    int naive_determinant(int n, int M[n][n])
```
The `naive_determinant` function is used to calculate the determinant of a given matrix using the brute-force approach. It uses a recursive algorithm based on the definition of a determinant. It returns an `int` consisting of the calculated determinant value.


## Timer
[<u>`timer.h`</u>](./timer.h)
```C
    int start ()
```
The `start` function returns an `int` representing the current time in milliseconds since the Epoch.

```C
    int end (int starttime)
```
The `end` function takes as an argument the `starttime` (as returned by `start()`) and returns the number of milliseconds since `starttime`.
