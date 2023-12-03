#include "lab3.h"

#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#include "error_handling.h"
#include "utils.h"

int ParentRoutine(const char* pathToChild, FILE* stream) {
    int fds[2] = {-1, -1};
    char* line = NULL;
    size_t len = 0;
    char* extraLine = NULL;
    size_t extraLen = 0;

    int sharedMemoryFd1 =
        shm_open(SHARED_MEMORY_NAME_1, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    GOTO_IF(sharedMemoryFd1 == -1, "shm_open", err);
    GOTO_IF(ftruncate(sharedMemoryFd1, MAP_SIZE) == -1, "ftruncate", err);
    int sharedMemoryFd2 =
        shm_open(SHARED_MEMORY_NAME_2, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    GOTO_IF(sharedMemoryFd2 == -1, "shm_open", err);
    GOTO_IF(ftruncate(sharedMemoryFd2, MAP_SIZE) == -1, "ftruncate", err);

    char* memptr1 = (char*)mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE,
                                MAP_SHARED, sharedMemoryFd1, 0);
    GOTO_IF(!memptr1, "mmap", err);
    char* memptr2 = (char*)mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE,
                                MAP_SHARED, sharedMemoryFd2, 0);
    GOTO_IF(!memptr2, "mmap", err);

    sem_t* wsemptr1 = sem_open(W_SEMAPHORE_1, O_CREAT, S_IRUSR | S_IWUSR, 1);
    sem_t* wsemptr2 = sem_open(W_SEMAPHORE_2, O_CREAT, S_IRUSR | S_IWUSR, 1);
    sem_t* revsemptr1 =
        sem_open(REV_SEMAPHORE_1, O_CREAT, S_IRUSR | S_IWUSR, 0);
    sem_t* revsemptr2 =
        sem_open(REV_SEMAPHORE_2, O_CREAT, S_IRUSR | S_IWUSR, 0);

    errno = 0;
    ssize_t nread = getline(&line, &len, stream);
    GOTO_IF(errno == ENOMEM, "getline", err);
    if (nread == -1) {
        fprintf(stderr, "Input 2 filenames\n");
        goto err;
    }
    line[nread - 1] = '\0';

    errno = 0;
    nread = getline(&extraLine, &extraLen, stream);
    GOTO_IF(errno == ENOMEM, "getline", err);
    if (nread == -1) {
        fprintf(stderr, "Input 2 filenames\n");
        goto err;
    }
    extraLine[nread - 1] = '\0';

    fds[0] = open(line, O_CREAT | O_WRONLY | O_TRUNC, MODE);
    GOTO_IF(fds[0] == -1, "open", err);
    fds[1] = open(extraLine, O_CREAT | O_WRONLY | O_TRUNC, MODE);
    GOTO_IF(fds[1] == -1, "open", err);
    free(extraLine);
    extraLine = NULL;

    pid_t pids[2];
    pids[0] = fork();
    GOTO_IF(pids[0] == -1, "fork", err);
    if (pids[0]) {
        pids[1] = fork();
        GOTO_IF(pids[1] == -1, "fork", err);
    }

    if (pids[0] == 0) {  // child1
        ABORT_IF(close(fds[1]), "close");
        fds[1] = -1;

        GOTO_IF(dup2(sharedMemoryFd1, STDIN_FILENO) == -1, "dup2", err);
        GOTO_IF(dup2(fds[0], STDOUT_FILENO) == -1, "dup2", err);
        ABORT_IF(close(fds[0]), "close");
        fds[0] = -1;

        GOTO_IF(execl(pathToChild, "child_lab3", W_SEMAPHORE_1, REV_SEMAPHORE_1,
                      NULL),
                "execl", err);
    } else if (pids[1] == 0) {  // child2
        ABORT_IF(close(fds[0]), "close");
        fds[0] = -1;
        GOTO_IF(dup2(sharedMemoryFd2, STDIN_FILENO) == -1, "dup2", err);

        GOTO_IF(dup2(fds[1], STDOUT_FILENO) == -1, "dup2", err);
        ABORT_IF(close(fds[1]), "close");
        fds[1] = -1;

        GOTO_IF(execl(pathToChild, "child_lab3", W_SEMAPHORE_2, REV_SEMAPHORE_2,
                      NULL),
                "execl", err);
    } else {  // parent
        ABORT_IF(close(fds[0]), "close");
        fds[0] = -1;
        ABORT_IF(close(fds[1]), "close");
        fds[1] = -1;
        while ((nread = getline(&line, &len, stream)) != -1) {
            if (nread <= FILTER_LEN) {
                sem_wait(wsemptr1);
                strncpy(memptr1 + 1, line, nread+1);
                memptr1[0] = 0;
                sem_post(revsemptr1);
            } else {
                sem_wait(wsemptr2);
                strncpy(memptr2 + 1, line, nread+1);
                memptr2[0] = 0;
                sem_post(revsemptr2);
            }
        }

        sem_wait(wsemptr1);
        memptr1[0] = -1;
        sem_post(revsemptr1);
        sem_wait(wsemptr2);
        memptr2[0] = -1;
        sem_post(revsemptr2);

        GOTO_IF(errno == ENOMEM, "getline", err);

        for (int i = 0; i < 2; ++i) {
            int status;
            int waitPid = wait(&status);
            GOTO_IF(status || !(waitPid == pids[0] || waitPid == pids[1]),
                    "wait", err);
        }

        free(line);
        sem_unlink(W_SEMAPHORE_1);
        sem_close(wsemptr1);
        sem_unlink(W_SEMAPHORE_2);
        sem_close(wsemptr2);
        sem_unlink(REV_SEMAPHORE_1);
        sem_close(revsemptr1);
        sem_unlink(REV_SEMAPHORE_2);
        sem_close(revsemptr2);
        shm_unlink(SHARED_MEMORY_NAME_1);
        shm_unlink(SHARED_MEMORY_NAME_2);
        munmap(memptr1, MAP_SIZE);
        munmap(memptr2, MAP_SIZE);
    }

    return 0;

err:
    free(line);
    free(extraLine);
    errno = 0;
    close(fds[0]);
    close(fds[1]);
    sem_unlink(W_SEMAPHORE_1);
    sem_close(wsemptr1);
    sem_unlink(W_SEMAPHORE_2);
    sem_close(wsemptr2);
    sem_unlink(REV_SEMAPHORE_1);
    sem_close(revsemptr1);
    sem_unlink(REV_SEMAPHORE_2);
    sem_close(revsemptr2);
    shm_unlink(SHARED_MEMORY_NAME_1);
    shm_unlink(SHARED_MEMORY_NAME_2);
    munmap(memptr1, MAP_SIZE);
    munmap(memptr2, MAP_SIZE);
    if (errno == EIO) {
        abort();
    }
    return -1;
}