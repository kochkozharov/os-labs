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
    char* ptr = (char*)mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
                            STDIN_FILENO, 0);
    SemaphorePair pair;

    GOTO_IF(CreateSemaphorePair(&pair, argv[1], argv[2]) == -1,
            "CreateSemaphorePair", err);
    while (1) {
        SemTimedWait(pair.revsemptr);
        if (ptr[0] == 0) {
            size_t len = strlen(ptr + 1);
            ReverseString(ptr + 1, len - 1);
            printf("%s", ptr + 1);
            sem_post(pair.wsemptr);
        } else {
            break;
        }
    }
    ABORT_IF(munmap(ptr, MAP_SIZE), "munmap");
    ABORT_IF(sem_close(pair.wsemptr), "sem_close");
    ABORT_IF(sem_close(pair.revsemptr), "sem_close");
    return 0;
err:
    ABORT_IF(munmap(ptr, MAP_SIZE), "munmap");
    ABORT_IF(sem_close(pair.wsemptr), "sem_close");
    ABORT_IF(sem_close(pair.revsemptr), "sem_close");
    return -1;
}