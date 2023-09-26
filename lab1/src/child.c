#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "error_handling.h"
#include "utils.h"

int main(void) {
    ssize_t nread;
    char *line = NULL;
    size_t len;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        ReverseString(line, nread-1);
        printf("%s", line);
    }
    GOTO_IF(errno == ENOMEM, "getline", err);
    free(line);
    return 0;

err:
    free(line);
    return -1;
}