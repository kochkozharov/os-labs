#include "utils.h"

#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

#include "error_handling.h"

void ReverseString(char* string, size_t length) {
    for (size_t i = 0; i < length >> 1; ++i) {
        char temp = string[i];
        string[i] = string[length - i - 1];
        string[length - i - 1] = temp;
    }
}

int CreateSharedMemory(SharedMemory* shm, const char* name, size_t size) {
    shm->fd = shm_open(name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shm->fd == -1) {
        return -1;
    }
    int status = ftruncate(shm->fd, (off_t)size);
    if (status == -1) {
        return -1;
    }
    shm->ptr =
        (char*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm->fd, 0);
    if (!shm->ptr) {
        return -1;
    }
    shm->name = name;
    shm->size = size;
    return 0;
}

void DestroySharedMemory(SharedMemory* shm) {
    ABORT_IF(shm_unlink(shm->name) == -1, "shm_unlink");
    ABORT_IF(munmap(shm->ptr, shm->size) == -1, "munmap");
}

int SemTimedWait(sem_t* sem) {
    struct timespec absoluteTime;
    if (clock_gettime(CLOCK_REALTIME, &absoluteTime) == -1) {
        return -1;
    }
    absoluteTime.tv_sec += 10;
    return sem_timedwait(sem, &absoluteTime);
}

int CreateSemaphorePair(SemaphorePair* pair, const char* wname,
                        const char* revname) {
    pair->wsemptr = sem_open(wname, O_CREAT, S_IRUSR | S_IWUSR, 1);
    if (!pair->wsemptr) {
        return -1;
    }
    pair->wsemname = wname;
    pair->revsemptr = sem_open(revname, O_CREAT, S_IRUSR | S_IWUSR, 0);
    if (!pair->revsemptr) {
        return -1;
    }
    pair->revsemname = revname;
    return 0;
}

void DestroySemaphorePair(SemaphorePair* pair) {
    ABORT_IF(sem_unlink(pair->wsemname) == -1, "sem_unlink");
    ABORT_IF(sem_close(pair->wsemptr) == -1, "sem_close");
    ABORT_IF(sem_unlink(pair->revsemname) == -1, "sem_unlink");
    ABORT_IF(sem_close(pair->revsemptr) == -1, "sem_close");
}