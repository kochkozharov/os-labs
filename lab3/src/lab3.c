#include "lab3.h"

#include <errno.h>
#include <fcntl.h>
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
        shm_open(SHARED_MEMORY_NAME_1, O_CREAT | O_WRONLY | O_TRUNC, MODE);
    GOTO_IF(sharedMemoryFd1 == -1, "shm_open", err);
    GOTO_IF(ftruncate(sharedMemoryFd1, MAP_SIZE) == -1, "ftruncate", err);
    int sharedMemoryFd2 =
        shm_open(SHARED_MEMORY_NAME_2, O_CREAT | O_WRONLY | O_TRUNC, MODE);
    GOTO_IF(sharedMemoryFd2 == -1, "shm_open", err);
    GOTO_IF(ftruncate(sharedMemoryFd2, MAP_SIZE) == -1, "ftruncate", err);

    char* memptr1 = (char*)mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE,
                                MAP_SHARED | MAP_ANONYMOUS, sharedMemoryFd1, 0);
    GOTO_IF(!memptr1, "mmap", err);
    char* memptr2 = (char*)mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE,
                                MAP_SHARED | MAP_ANONYMOUS, sharedMemoryFd2, 0);
    GOTO_IF(!memptr2, "mmap", err);

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

        GOTO_IF(execl(pathToChild, "child", NULL), "execl", err);
    } else if (pids[1] == 0) {  // child2
        ABORT_IF(close(fds[0]), "close");
        fds[0] = -1;
        GOTO_IF(dup2(sharedMemoryFd2, STDIN_FILENO) == -1, "dup2", err);

        GOTO_IF(dup2(fds[1], STDOUT_FILENO) == -1, "dup2", err);
        ABORT_IF(close(fds[1]), "close");
        fds[1] = -1;

        GOTO_IF(execl(pathToChild, "child", NULL), "execl", err);
    } else {  // parent
        ABORT_IF(close(fds[0]), "close");
        fds[0] = -1;
        ABORT_IF(close(fds[1]), "close");
        fds[1] = -1;

        ssize_t nread;
        sigset_t sigset;
        sigemptyset(&sigset);
        sigaddset(&sigset, SIGUSR1);
        sigprocmask(SIG_SETMASK, &sigset, NULL);
        while ((nread = getline(&line, &len, stream)) != -1) {
            if (nread <= FILTER_LEN) {
                memcpy(memptr1, line, nread);
                kill(pids[0], SIGUSR1);
                int sig;
                sigwait(&sigset, &sig);
            } else {
                memcpy(memptr2, line, nread);
                kill(pids[1], SIGUSR1);
                int sig;
                sigwait(&sigset, &sig);
            }
        }
        GOTO_IF(errno == ENOMEM, "getline", err);
        kill(pids[0], SIGUSR2);
        kill(pids[1], SIGUSR2);

        for (int i = 0; i < 2; ++i) {
            int status;
            int waitPid = wait(&status);
            GOTO_IF(status || !(waitPid == pids[0] || waitPid == pids[1]),
                    "wait", err);
        }
        free(line);
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
    if (errno == EIO) {
        abort();
    }
    return -1;
}