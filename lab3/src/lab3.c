#include "lab3.h"

#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "error_handling.h"
#include "utils.h"

int ParentRoutine(const char* pathToChild, FILE* stream) {
    int fds[2] = {-1, -1};
    char* line = NULL;
    size_t len = 0;
    char* extraLine = NULL;
    size_t extraLen = 0;

    SharedMemory shm1;
    SharedMemory shm2;
    GOTO_IF(CreateSharedMemory(&shm1, SHARED_MEMORY_NAME_1, MAP_SIZE),
            "CreateSharedMemory", err);
    GOTO_IF(CreateSharedMemory(&shm2, SHARED_MEMORY_NAME_2, MAP_SIZE),
            "CreateSharedMemory", err);

    SemaphorePair pair1;
    SemaphorePair pair2;

    GOTO_IF(CreateSemaphorePair(&pair1, W_SEMAPHORE_NAME_1,
                                REV_SEMAPHORE_NAME_1) == -1,
            "CreateSemaphorePair", err);
    GOTO_IF(CreateSemaphorePair(&pair2, W_SEMAPHORE_NAME_2,
                                REV_SEMAPHORE_NAME_2) == -1,
            "CreateSemaphorePair", err);

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
        close(fds[1]);
        fds[1] = -1;

        GOTO_IF(dup2(shm1.fd, STDIN_FILENO) == -1, "dup2", err);
        GOTO_IF(dup2(fds[0], STDOUT_FILENO) == -1, "dup2", err);
        close(fds[0]);
        fds[0] = -1;

        GOTO_IF(execl(pathToChild, "child_lab3", pair1.wsemname,
                      pair1.revsemname, NULL),
                "execl", err);
    } else if (pids[1] == 0) {  // child2
        close(fds[0]);
        fds[0] = -1;

        GOTO_IF(dup2(shm2.fd, STDIN_FILENO) == -1, "dup2", err);
        GOTO_IF(dup2(fds[1], STDOUT_FILENO) == -1, "dup2", err);
        close(fds[1]);
        fds[1] = -1;

        GOTO_IF(execl(pathToChild, "child_lab3", pair2.wsemname,
                      pair2.revsemname, NULL),
                "execl", err);
    } else {  // parent
        close(fds[0]);
        fds[0] = -1;
        close(fds[1]);
        fds[1] = -1;
        while ((nread = getline(&line, &len, stream)) != -1) {
            if (nread <= FILTER_LEN) {
                GOTO_IF(SemTimedWait(pair1.wsemptr) == -1, "semTimeout", err);
                strncpy(shm1.ptr + 1, line, nread + 1);
                shm1.ptr[0] = 0;
                sem_post(pair1.revsemptr);
            } else {
                GOTO_IF(SemTimedWait(pair2.wsemptr) == -1, "semTimeout", err);
                strncpy(shm2.ptr + 1, line, nread + 1);
                shm2.ptr[0] = 0;
                sem_post(pair2.revsemptr);
            }
        }

        GOTO_IF(SemTimedWait(pair1.wsemptr) == -1, "semTimeout", err);
        GOTO_IF(SemTimedWait(pair2.wsemptr) == -1, "semTimeout", err);
        shm1.ptr[0] = -1;
        shm2.ptr[0] = -1;
        sem_post(pair2.revsemptr);
        sem_post(pair1.revsemptr);

        GOTO_IF(errno == ENOMEM, "getline", err);

        for (int i = 0; i < 2; ++i) {
            int status;
            int waitPid = wait(&status);
            GOTO_IF(status || !(waitPid == pids[0] || waitPid == pids[1]),
                    "wait", err);
        }

        free(line);
        DestroySemaphorePair(&pair1);
        DestroySemaphorePair(&pair2);
        DestroySharedMemory(&shm1);
        DestroySharedMemory(&shm2);
    }

    return 0;

err:
    free(line);
    free(extraLine);
    errno = 0;
    close(fds[0]);
    close(fds[1]);
    DestroySemaphorePair(&pair1);
    DestroySemaphorePair(&pair2);
    DestroySharedMemory(&shm1);
    DestroySharedMemory(&shm2);
    if (errno == EIO) {
        abort();
    }
    return -1;
}