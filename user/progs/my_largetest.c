#include <syscall.h>
#include <simics.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread.h>
#include <thrgrp.h>
#include <assert.h>
#include <mutex.h>

void* func(void *arg) {
    while (1) {
        int res = thr_create(func, NULL);
        if (res < 0) {
            exit(0);
        }
    }
}

int main() {
    thr_init(4 * 1024);
    while (1) {
        int res = thr_create(func, NULL);
        if (res < 0) {
            return 0;
        }
    }
}