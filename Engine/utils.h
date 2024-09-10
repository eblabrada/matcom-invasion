#ifndef UTILS_H
#define UTILS_H

#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

// return milliseconds
long get_ticks(void);

int LOG10(int v);

void throw_error(const char* message);

#endif // UTILS_H