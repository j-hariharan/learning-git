# Multithread
This section uses [**multi-threading concept**](../prerequisites/readme.md#creating-threads) to implement multi-programming for calculating the determinant of a matrix. As threads share the code and data section of the process, and since it is possible to pass arguments to threads, there is no need for specific methods of _inter-process communication_ in this case. However, there are still two possible stratergies by which to share data between child and parent threads:

1. [Separate shared variable for each child thread](#separate-shared-variables)
2. [Single shared variable for all child threads](#single-shared-variable)


### Max Times
`MAX_TIMES` is a marco used in both the programs for multiprocessing. This variable is used to limit the number of processes that are created during the execution of the program. It specifies the maximum depth of recursive calls for which `fork()` will be called to create child processes. After the specified recursion depth is reached, the determinant is calculated using [`naive_determinant()`](../helpers/readme.md#naive-determinant-calculator) in a single process.


### Note on using `malloc()` to pass arguments to thread
Since passing an argument to `pthread_create()` only passes the address of the `args` struct, it is important to make sure that the data at that address is persistent. This is because the execution of the thread may start at a later time, and the `args` argument should not be overwritten or deallocated in that time. For this reason, _dynamic memory allocation_ is used with the help of `malloc()`.



## Separate Shared Variables

[<u>`separate_variables.c`</u>](./separate_variables.c)

This implementation of multi-threading creates an array in the parent thread, each element of which is used to save data from one of the child processes. The final determinant is then computed from these individual determinants in the parent thread itself, after all the child threads have completed their execution.
```C
    // array to store result of each smaller matrix
    int results[n];
    // for each child process (j = 0 to (n-1))
    a1->result = &results[j];

    // in the child process
    *result = computed_determinant;
```
This way, each child process saves to a separate address, avoiding any race conditions. This implementation, thus, does not require the use of process synchronization techniques, but it does take up more space when compared to using a single shared variable.


## Single Shared Variable

[<u>`single_variable.c`</u>](./single_variable.c)

This implementation of multi-threading, similar to [shared variables in multi-processing](../multiprocess/readme.md#shared-memory), creates a single variable which is passed to every child thread. The child thread, after computing its respective determinant, updates this shared variable, adding the respective cofactor to it. In this way, no extra computation takes place in the parent thread, which simply waits for the child processes to complete execution before returning.
```C
    // create and init new shared variable
    int newres = 0;
    // for each child process (j = 0 to (n-1))
    a1->result = &newres;
    a1->coeff = M[j];

    // in the child process
    *result += coeff * newres;
```
Since many processes are reading, writing and updating the same variable `result` simultaneously, this can lead to [**race condition**](../prerequisites/readme.md#2-in-multiple-threads). To tackle this problem, [**process synchronization**](../prerequisites/readme.md#2-in-multiple-threads-1) should be implemented to properly handle the critical section. 
```C
    sem_t newmutex;
    sem_init(&newmutex, 0, 1);

    // for each child process
    a1->mutex = &newmutex;
```
For this, a `mutex` lock is created in every parent thread and is then passed to the child thread, as shown above. The child thread has to acquire this lock before executing the critical section, and release it after the critical section is complete.
```C
    // update shared result
    sem_wait(mutex);
        // critical section
        *result += coeff * newres;
    sem_post(mutex);
```