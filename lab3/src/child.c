#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "error_handling.h"
#include "utils.h"

int main(int argc, char* argv[]) {
    (void)argc;
    char* memptr = (char*)mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE,
                               MAP_SHARED, STDIN_FILENO, 0);
    GOTO_IF(!memptr, "mmap", err);
    sem_t* wsemptr = sem_open(argv[1], 0);
    GOTO_IF(!wsemptr, "sem_open", err);
    sem_t* revsemptr = sem_open(argv[2], 0);
    GOTO_IF(!revsemptr, "sem_open", err);

    while (1) {
        struct timespec ts;
        GOTO_IF(clock_gettime(CLOCK_REALTIME, &ts) == -1, "clock_gettime", err);
        ts.tv_sec += 1;
        GOTO_IF(sem_timedwait(revsemptr, &ts) == -1, "sem_timeout", err);

        if (memptr[0] == 0) {
            size_t len = strlen(memptr + 1);
            ReverseString(memptr + 1, len - 1);
            printf("%s", memptr + 1);
            sem_post(wsemptr);
        } else {
            break;
        }
    }
    sem_close(revsemptr);
    sem_close(wsemptr);
    munmap(memptr, MAP_SIZE);
    return 0;
err:
    sem_close(revsemptr);
    sem_close(wsemptr);
    munmap(memptr, MAP_SIZE);
    return -1;
}