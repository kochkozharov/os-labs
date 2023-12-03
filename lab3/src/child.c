#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#include "error_handling.h"
#include "utils.h"

int main(int argc, char* argv[]) {
    (void)argc;
    char* memptr = (char*)mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE,
                               MAP_SHARED, STDIN_FILENO, 0);
    sem_t* wsemptr = sem_open(argv[1], 0);
    sem_t* revsemptr = sem_open(argv[2], 0);
    while (1) {
        sem_wait(revsemptr);

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
}