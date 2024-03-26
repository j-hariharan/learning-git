# Multiprocess
This section uses [**multi-processing concept** ](../prerequisites/readme.md#creating-processes) (`fork()` system call) to achieve parallelization in calculating the determinant of a matrix. Two methods can be used to implement inter-process communication in this case:
1. [Pipes](#pipes)
2. [Shared Memory](#shared-memory)


### Max Times
`MAX_TIMES` is a marco used in both the programs for multiprocessing. This variable is used to limit the number of processes that are created during the execution of the program. It specifies the maximum depth of recursive calls for which `fork()` will be called to create child processes. After the specified recursion depth is reached, the determinant is calculated using [`naive_determinant()`](../helpers/readme.md#naive-determinant-calculator) in a single process.


## Pipes

[<u>`pipes.c`</u>](./pipes.c)

This implementation of multiprocessing uses [**pipes**](../prerequisites/readme.md#2-using-pipes) for inter-process communication. Pipes are a one-way stream of data from the "write end" to the "read end". As pipes only allow for communication in one direction, it can only be used to communicate between a single child process and its parent process. Thus, an array of pipes need to be made to communicate with all the child processes.
```C
    // Array of pipes
    int fds[n][2];

    // for every j from 0 to (n-1)
    pipe(fds[j]);
```
The child and parent processes communicate using the `write()` and `read()` functions on `fds[j][1]` (called the "write end") and `fds[j][0]` (called the "read end") respectively.

Since only two processes can communicate unidirectionally using pipes, there is no requirement for process synchronization in this case. But since one pipe has to be made for every child process, this can lead to wastage of memory and other resources.

## Shared Memory

[<u>`shared_memory.c`</u>](./shared_memory.c)

This implementation uses [**shared memory**](../prerequisites/readme.md#1-using-shared-memory) for inter-process communication. Shared memory are special memory addresses allotted by the operating system for use across several processes. These processes may be spawned using `fork()` or even separately, and need not run at the same time. Shared memory are very flexible as they allow for bi-directional communication.
```C
    int shmid = shmget((key_t) key, sizeof(int), 0666 | IPC_CREAT);
    int* result = (int*) shmat(shmid, (void*)0, 0);
```
In the shared memory implementation, a shared `result` variable is created which is then updated by the child processes, adding each cofactor to it. At the end, the `result` variable has the final value of the determinant, and is returned from the function.

The creation of a new shared memory requires a unique `key` value which has not been used for creating other shared variables for the same use. To achieve this, it becomes necessary to keep a count of how many shared variables have been created. Hence, a special shared variable `shmcount` is created and passed to every function call. It is incremented every time that a new shared variable is created.

Since many processes are reading and writing to the same shared memory address simultaneously, this will lead to the occurance of [**race condition**](../prerequisites/readme.md#race-condition). To overcome this challenge, [**process synchronization**](../prerequisites/readme.md#1-in-multiple-processes-1) is used. To create the `mutex` lock, a new shared variable is required.
```C
    int semshmid = shmget((key_t) key, sizeof(sem_t), 0666 | IPC_CREAT);
    sem_t* mutex = (sem_t*) shmat(semshmid, (void*)0, 0);
    sem_init(mutex, 1, 1);
```
A similar lock is also required for the `shmcount` shared variable.

These lock variables are used for the execution of critical section, which usually involves incrementing a shared variable. The function `sem_wait()` is called in the entry section and `sem_post()` is called in the exit section.

<u>For `result` shared variable</u>

```C
    sem_wait(mutex);
        (*result) += res * M[0][j];
    sem_post(mutex);
```

<u>For `shmcount` shared variable</u>

```C
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
```