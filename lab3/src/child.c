#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#include "error_handling.h"
#include "utils.h"

int main(void) {
    ssize_t nread;
    char* memptr = (char*)mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE,
                               MAP_SHARED, 1, 0);
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);
    sigaddset(&sigset, SIGUSR2);
    int sig;
    sigprocmask(SIG_SETMASK, &sigset, NULL);
    while (1) {
        sigwait(&sigset, &sig);
        if (sig == SIGUSR1) {
            size_t len = strlen(memptr);
            ReverseString(memptr, len - 1);
            kill(getppid(), SIGUSR1);
            printf("%s", memptr);
        } else {
            break;
        }
    }
    munmap(memptr,MAP_SIZE);
    return 0;
}