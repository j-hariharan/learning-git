#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

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
