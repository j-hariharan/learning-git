# Prerequisites
The following programs were implemented to revise the theory concepts involved in multiprogramming.

## 1. Creating processes:

[<u>`fork.c`</u>](./fork.c)
```C
int main () {
    pid_t pid = fork();

    if (pid == 0) {
        printf("Child process\n");
    } else {
        printf("Parent process... child PID is %d\n", pid);
        pid_t wpid = wait(NULL);
        printf("Waited for child process of PID %d\n", wpid);
    }
}
```

## 2. Creating threads:

[<u>`threads.c`</u>](./threads.c)
```C
void* sayHello (void* arg) {
    printf("From thread!\n");
}

void main () {
    pthread_t p1;

    pthread_create(&p1, NULL, sayHello, NULL);
    printf("From parent process!\n");
    pthread_join(p1, NULL);

    printf("Joined the thread!\n\n");
}
```

## 3. Inter-process communication

### using shared memory

[<u>`shared_memory.c`</u>](./shared_memory.c)
```C
int main () {
    int shmid = shmget((key_t) 1, sizeof(int), 0666 | IPC_CREAT);
    int* n = (int*) shmat(shmid, (void*)0, 0);
    *n = 100;

    pid_t pid = fork();

    if (pid == 0) {
        printf("Child process\n");
        printf("Value of n = %d... updating to 200\n", *n);
        *n = 200;
    } else {
        printf("Parent process\nWaiting for child process to complete execution\n");
        wait(NULL);
        printf("Value of n from parent process = %d\n", *n);

        shmdt(n);
        shmctl(shmid, IPC_RMID, NULL);
    }
}
```

### using pipes

[<u>`pipes.c`</u>](./pipes.c)
```C
int main () {
    int n[2];
    pipe(n);

    pid_t pid = fork();

    if (pid == 0) {
        close(n[0]);
        printf("Child process... Writing value 200 to pipe\n");
        int value = 200;
        write(n[1], &value, sizeof(int));

    } else {
        close(n[1]);
        printf("Parent process\nWaiting for child process to complete execution\n");
        wait(NULL);
        printf("Reading from pipe...\n");
        int value;
        read(n[0], &value, sizeof(int));
        printf("Read value %d in parent process\n\n", value);
    }
}
```

## 4. Race condition
The below codes do not produce the expected output of `0` due to race condition.

### in multiple processes

[<u>`race_processes.c`</u>](./race_processes.c)
```C
int main () {
    int shmid = shmget((key_t) 1, sizeof(int), 0666 | IPC_CREAT);
    int* n = (int*) shmat(shmid, (void*)0, 0);
    *n = 0;

    pid_t pid = fork();

    if (pid == 0) {
        printf("Child process\n");
        for (int i=0 ; i<100000 ; i++) {
            (*n)--;
        }
    } else {
        printf("Parent process\n");
        for (int i=0 ; i<100000 ; i++) {
            (*n)++;
        }
        wait(NULL);
        printf("Final value of n: %d (expected 0)\n", *n);
        shmdt(n);
        shmctl(shmid, IPC_RMID, NULL);
    }
}
```

### in multiple threads

[<u>`race_threads.c`</u>](./race_processes.c)
```C
void* add (void* arg) {
    int* n = (int*) arg;

    for (int i=0 ; i<100000 ; i++) {
        (*n)++;
    }
}

void* subtract (void* arg) {
    int* n = (int*) arg;

    for (int i=0 ; i<100000 ; i++) {
        (*n)--;
    }
}

void main () {
    int n = 0;

    pthread_t p1, p2;
    pthread_create(&p1, NULL, add, &n);
    pthread_create(&p2, NULL, subtract, &n);

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);

    printf("Final value of n: %d (expected 0)\n", n);
}
```

## 5. Process Synchronization using Semaphores

### in multiple processes

[<u>`semaphore_processes.c`</u>](./semaphore_processes.c)
```C
int main () {
    int shmid = shmget((key_t) 1, sizeof(int), 0666 | IPC_CREAT);
    int* n = (int*) shmat(shmid, (void*)0, 0);
    *n = 0;

    int semshmid = shmget((key_t) 2, sizeof(sem_t), 0666 | IPC_CREAT);
    sem_t* mutex = (sem_t*) shmat(semshmid, (void*)0, 0);
    sem_init(mutex, 1, 1);

    pid_t pid = fork();

    if (pid == 0) {
        printf("Child process\n");
        for (int i=0 ; i<100000 ; i++) {
            sem_wait(mutex);
            (*n)--;
            sem_post(mutex);
        }
        shmdt(n);
    } else {
        printf("Parent process\n");
        for (int i=0 ; i<100000 ; i++) {
            sem_wait(mutex);
            (*n)++;
            sem_post(mutex);
        }

        wait(NULL);
        printf("Final value of n: %d (expected 0)\n", *n);
        shmdt(n);
        shmctl(shmid, IPC_RMID, NULL);

        shmdt(mutex);
        shmctl(semshmid, IPC_RMID, NULL);
    }
}
```

### in multiple threads

[<u>`semaphore_threads.c`</u>](./semaphore_threads.c)
```C
void* add (void* arg) {
    args* a = (args*) arg;
    int* n = a->n;
    sem_t* mutex = a->mutex;

    for (int i=0 ; i<100000 ; i++) {
        sem_wait(mutex);
        (*n)++;
        sem_post(mutex);
    }
}

void* subtract (void* arg) {
    struct args* a = (struct args*) arg;
    int* n = a->n;
    sem_t* mutex = a->mutex;

    for (int i=0 ; i<100000 ; i++) {
        sem_wait(mutex);
        (*n)--;
        sem_post(mutex);
    }
}

void main () {
    int n = 0;
    sem_t mutex;
    sem_init(&mutex, 0, 1);

    args arg = {&n, &mutex};

    pthread_t p1, p2;
    pthread_create(&p1, NULL, add, &arg);
    pthread_create(&p2, NULL, subtract, &arg);

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);

    printf("Final value of n: %d (expected 0)\n", n);
}
```