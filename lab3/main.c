#include <stdio.h>
#include <stdlib.h>

#include "lab3.h"

int main(void) {
    const char *childPath = getenv("PATH_TO_CHILD");
    if (!childPath) {
        fprintf(stderr, "Set environment variable PATH_TO_CHILD\n");
        exit(EXIT_FAILURE);
    }
    return ParentRoutine(childPath , stdin);
}