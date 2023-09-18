#include "test.h"

#include <stdio.h>
#include <stdlib.h>

int CreateProcess() {
    pid_t pid = fork();
    if (-1 == pid) {
        perror("fork");
        exit(-1);
    }
    return pid;
}