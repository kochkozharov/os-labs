#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#include <stdbool.h> // false
#include <stdio.h> // fflush, fprintf, perror, stderr, vfprintf
#include <stdlib.h> // EXIT_FAILURE, abort, exit
#include <string.h> // strerror

#define ABORT_IF(condition, err_func) \
    do { \
        if ((condition)) { \
            fprintf(stderr, "%s:%d: ", __FILE__, __LINE__); \
            perror(err_func); \
            abort(); \
        } \
    } while (false)

#define EXIT_IF(condition, err_func) \
    do { \
        if ((condition)) { \
            fprintf(stderr, "%s:%d: ", __FILE__, __LINE__); \
            perror(err_func); \
            exit(EXIT_FAILURE); \
        } \
    } while (false)

#ifdef NDEBUG
#define GOTO_IF(condition, label) \
    do { \
        if ((condition)) \
            goto label; \
    } while (false)
#else
#define GOTO_IF(condition, err_func, label) \
    do { \
        if ((condition)) { \
            fprintf(stderr, "%s:%d: ", __FILE__, __LINE__); \
            if (err_func) { \
                perror(err_func); \
            } \
            goto label; \
        } \
    } while (false)
#endif // NDEBUG

#endif // ERROR_HANDLING_H
