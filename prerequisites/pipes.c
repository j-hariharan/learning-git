#include<stdio.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>

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
