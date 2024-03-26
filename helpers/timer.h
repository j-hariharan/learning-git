#include<stdio.h>
#include<sys/time.h>

int start () {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec*1000 + t.tv_usec/1000;
}

int end (int starttime) {
    return start() - starttime;
}