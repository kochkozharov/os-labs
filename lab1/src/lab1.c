#include "lab1.h"

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "error_handling.h"
#include "utils.h"

int ParentRoutine(const char* pathToChild, FILE* stream) {
    int fds[2] = {-1, -1};
    int pipes[2][2] = {{-1, -1}, {-1, -1}};

    char* line = NULL;
    size_t len;
    char* extraLine = NULL;
    size_t extraLen;
    
    errno = 0;
    ssize_t nread = getline(&line, &len, stream);
    GOTO_IF(errno == ENOMEM, "getline" , err);
    if (nread == -1) {
        fprintf(stderr, "Input 2 filenames\n");
        goto err;
    }
    line[nread - 1] = '\0';

    errno = 0;
    nread = getline(&extraLine, &extraLen, stream);
    GOTO_IF(errno == ENOMEM, "getline" , err);
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

    GOTO_IF(pipe(pipes[0]), "pipe", err);
    GOTO_IF(pipe(pipes[1]), "pipe", err);

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
        ABORT_IF(close(pipes[1][READ_END]), "close");
        pipes[1][READ_END] = -1;
        ABORT_IF(close(pipes[1][WRITE_END]), "close");
        pipes[1][WRITE_END] = -1;
        ABORT_IF(close(pipes[0][WRITE_END]), "close");
        pipes[0][WRITE_END] = -1;

        GOTO_IF(dup2(pipes[0][READ_END], STDIN_FILENO) == -1, "dup2", err);
        ABORT_IF(close(pipes[0][READ_END]), "close");
        pipes[0][READ_END] = -1;

        GOTO_IF(dup2(fds[0], STDOUT_FILENO) == -1, "dup2", err);
        ABORT_IF(close(fds[0]), "close");
        fds[0] = -1;

        GOTO_IF(execl(pathToChild, "child", NULL), "execl", err);
    } else if (pids[1] == 0) {  // child2
        ABORT_IF(close(fds[0]), "close");
        fds[0] = -1;
        ABORT_IF(close(pipes[0][READ_END]), "close");
        pipes[0][READ_END] = -1;
        ABORT_IF(close(pipes[0][WRITE_END]), "close");
        pipes[0][WRITE_END] = -1;
        ABORT_IF(close(pipes[1][WRITE_END]), "close");
        pipes[1][WRITE_END] = -1;

        GOTO_IF(dup2(pipes[1][READ_END], STDIN_FILENO) == -1, "dup2", err);
        ABORT_IF(close(pipes[1][READ_END]), "close");
        pipes[1][READ_END] = -1;

        GOTO_IF(dup2(fds[1], STDOUT_FILENO) == -1, "dup2", err);
        ABORT_IF(close(fds[1]), "close");
        fds[1] = -1;

        GOTO_IF(execl(pathToChild, "child", NULL), "execl", err);
    } else {  // parent
        ABORT_IF(close(pipes[0][READ_END]), "close");
        pipes[0][READ_END] = -1;
        ABORT_IF(close(pipes[1][READ_END]), "close");
        pipes[1][READ_END] = -1;
        ABORT_IF(close(fds[0]), "close");
        fds[0] = -1;
        ABORT_IF(close(fds[1]), "close");
        fds[1] = -1;

        GOTO_IF(waitpid(-1, NULL, WNOHANG), "waitpid", err);
        ssize_t nread;
        size_t len;
        while ((nread = getline(&line, &len, stream)) != -1) {
            GOTO_IF(waitpid(-1, NULL, WNOHANG), "waitpid", err);
            if (nread <= FILTER_LEN) {
                GOTO_IF(write(pipes[0][WRITE_END], line, nread) == -1, "write", err);
            } else {
                GOTO_IF(write(pipes[1][WRITE_END], line, nread) == -1, "write", err);
            }
        }
        GOTO_IF(errno == ENOMEM, "getline", err);

        ABORT_IF(close(pipes[0][WRITE_END]), "close");
        pipes[0][WRITE_END] = -1;
        ABORT_IF(close(pipes[1][WRITE_END]), "close");
        pipes[1][WRITE_END] = -1;

        for (int i = 0; i < 2; ++i) {
            int status;
            int waitPid = wait(&status);
            GOTO_IF(status || !(waitPid == pids[0] || waitPid == pids[1]), "wait", err);
        }
    }

    free(line);
    return 0;

err:
    free(line);
    free(extraLine);
    errno = 0;
    close(fds[0]);
    close(fds[1]);
    close(pipes[0][READ_END]);
    close(pipes[0][WRITE_END]);
    close(pipes[1][READ_END]);
    close(pipes[1][WRITE_END]);
    if (errno == EIO) {
        abort();
    }
    return -1;
}