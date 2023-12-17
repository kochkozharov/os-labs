#ifndef LAB1_UTILS_H
#define LAB1_UTILS_H

#include <semaphore.h>
#include <stddef.h>

#define MAP_SIZE (1024U)
#define SHARED_MEMORY_NAME_1 "shared_memory_1"
#define SHARED_MEMORY_NAME_2 "shared_memory_2"
#define W_SEMAPHORE_NAME_1 "w_semaphore_1"
#define W_SEMAPHORE_NAME_2 "w_semaphore_2"
#define REV_SEMAPHORE_NAME_1 "rev_semaphore_1"
#define REV_SEMAPHORE_NAME_2 "rev_semaphore_2"

enum PipeEnd { READ_END, WRITE_END };

typedef struct {
    const char* name;
    char* ptr;
    size_t size;
    int fd;
} SharedMemory;

typedef struct {
    sem_t* wsemptr;
    const char* wsemname;
    sem_t* revsemptr;
    const char* revsemname;
} SemaphorePair;

int CreateSharedMemory(SharedMemory* shm, const char* name, size_t size);
void DestroySharedMemory(SharedMemory* shm);
int SemTimedWait(sem_t* sem);
int CreateSemaphorePair(SemaphorePair* pair, const char* wname,
                        const char* revname);
void DestroySemaphorePair(SemaphorePair* pair);
void ReverseString(char* string, size_t length);

#endif  // LAB1_UTILS_H