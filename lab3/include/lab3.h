#ifndef LAB1_H
#define LAB1_H

#include <stdio.h>

#define FILTER_LEN (11U)
#define MODE (0644U)



int ParentRoutine(const char* pathToChild, FILE* stream);

#endif  // LAB1_H