#include "lab1.h"

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "error_handling.h"
#include "utils.h"

int ParentRoutine(const char* pathToChild, FILE* stream) {
    ssize_t nread;
    char* line = NULL;
    size_t len = 0;
    int fds[2] = {-1, -1};
    int pipes[2][2] = {{-1, -1}, {-1, -1}};
    for (int i = 0; i < 2; ++i) {
        nread = getline(&line, &len, stream);
        GOTO_IF(nread == -1, "getline", err);
        line[nread - 1] = '\0';
        fds[i] = open(line, O_CREAT | O_RDWR | O_TRUNC, MODE);
        GOTO_IF(fds[i] == -1, "open", err);
    }
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
        GOTO_IF(dup2(fds[0], STDOUT_FILENO) == -1, "dup2", err);
        execl(pathToChild, "child", NULL);
        goto err;
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
        GOTO_IF(dup2(fds[1], STDOUT_FILENO) == -1, "dup2", err);
        execl(pathToChild, "child", NULL);
        goto err;
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
        while ((nread = getline(&line, &len, stream)) != -1) {
            GOTO_IF(waitpid(-1, NULL, WNOHANG), "waitpid", err);
            if (nread <= FILTER_LEN) {
                GOTO_IF(write(pipes[0][WRITE_END], line, nread)==-1, "write", err);
            } else {
                GOTO_IF(write(pipes[1][WRITE_END], line, nread)==-1, "write", err);
            }
        }
        GOTO_IF(errno == ENOMEM, "getline", err);
        ABORT_IF(close(pipes[0][WRITE_END]), "close");
        pipes[0][WRITE_END] = -1;
        ABORT_IF(close(pipes[1][WRITE_END]), "close");
        pipes[1][WRITE_END] = -1;
    }
    free(line);
    return 0;

err:
    free(line);
    close(fds[0]);
    close(fds[1]);
    close(pipes[0][READ_END]);
    close(pipes[0][WRITE_END]);
    close(pipes[1][READ_END]);
    close(pipes[1][WRITE_END]);
    return -1;
}