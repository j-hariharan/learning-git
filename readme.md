### IT253 Project
# Calculating Determinant using Multiprogramming Techniques

This project aims at optimising the brute force algorithm for computing the determinant of a given matrix by using the concepts of multiprogramming. We have explored several techniques to solve this problem, learning about their advantages and disadvantages along the way. The goal is not to come up with the best or most efficient program to calculate the determinant of a matrix, but rather to understand and implement some core concepts which will prove to be useful in a vast variety of other similar problems.


## Index
1. [Prerequisites](./prerequisites/readme.md)
2. [Helpers](./helpers/readme.md)
3. [Multiprocess](./multiprocess/readme.md)
4. [Multithread](./multithread/readme.md)


## Result

### Multi-programming
Various techniques to achieve multi-programming and parallel processing were studied and compared, each with their own pros and cons. The multi-programming paradigms used were:
1. [Multi-processing](./multiprocess/readme.md)
2. [Multi-threading](./multithread/readme.md)

These were compared against the single-process [`naive_determinant()`](./helpers/readme.md#naive-determinant-calculator) function, comparing running time as well as complexity in implementation. 

- The main difference between multi-threading and multi-processing is that threads share the same code and data segment. Hence, any global variables or dynamically allocated memory is automatically shared between threads. For processes, on the other hand, sharing memory segments requires system calls like [`pipe`](./prerequisites/readme.md#2-using-pipes) or [`shmget`](./prerequisites/readme.md#1-using-shared-memory).

- Processes, however, are completed isolated from each other, and hence, cannot interfere with the working of each other. It provides safety in that it is not possible able to unintentionally modify variables which are not meant to be shared. Also, since the heap and stack data are all copied to the new process, there is no need to worry about [accidentally overwriting useful values](./multithread/readme.md#note-on-using-malloc-to-pass-arguments-to-thread). Faults are also isolated, so if one process fails, the others will continue to run in a multi-process program.
- Both multi-processing and multi-threading should be used judiciously. They do cause overhead for the operating system or kernel, while creating them as well as during context-switches. It is careful to consider this cost, comparing it against the gains of parallelization.


### Data Sharing
Two classes of data sharing were used along the way:

1. Using separate spaces of shared memory for each child [process](./multiprocess/readme.md#pipes)/[thread](./multithread/readme.md#separate-shared-variables).

2. Using the same shared memory space for each child [process](./multiprocess/readme.md#shared-memory)/[thread](./multithread/readme.md#single-shared-variable).

These, too, have their differences, and are preferred based on the specific use case:
- Using separate space of shared memory for each child is a simpler implementation as it ensures that only one child will update any given address space. This eliminates the possibility of any inconsistency due to race conditions, and hence does not require any process synchronization techniques.
- Using the same shared memory space for all children is more space-efficient. Also, any expensive computations involved in the aggregating the results from the children will also be carried out in the child processing, increasing efficiency. But this implementation necessitates the use of [**process synchronization**](./prerequisites/readme.md#process-synchronization-using-semaphores). This can add additional overhead for the program, and lead to a new set of problems like _starvation_ and _deadlock_.
- A choice needs to be made among the two options depending upon the availability of memory, size of the shared variable, number of child processes, complexity of computations, etc.


### Running Time Comparision

```sh
# multi-process
/$ cd multiprocess/
/multiprocess$ gcc shared_memory.c 
/multiprocess$ ./a.out 10

Determinant: 1187029852 (40ms)
Naive determinant: 1187029852 (71ms)

/multiprocess$ ./a.out 11

Determinant: 53554124 (226ms)
Naive determinant: 53554124 (699ms)

/multiprocess$ ./a.out 12

Determinant: -1424335653 (2473ms)
Naive determinant: -1424335653 (9171ms)

# multi-thread
/multiprocess$ cd ../multithread/
/multithread$ gcc single_variable.c 
/multithread$ ./a.out 10

Determinant: 1187029852 (40ms)
Naive determinant: 1187029852 (68ms)

/multithread$ ./a.out 11

Determinant: 53554124 (214ms)
Naive determinant: 53554124 (752ms)

/multithread$ ./a.out 12

Determinant: -1424335653 (2551ms)
Naive determinant: -1424335653 (9038ms)
```

Some of the running times of the multiprogramming implementations in comparision with the naive implementation is shown above.
- It is seen that both multi-programming techniques gives similar results which are also significantly better when compared to the single-process approach.
- The type of data sharing used does not affect the running time in any noticable way. Both methods can be considred equally efficient.
- It is worth noting that the single-process approach has better performance when it comes to smaller input sizes. This is because the overhead of creating and managing threads/processes becomes more expensive than the improvements due to parallelization.