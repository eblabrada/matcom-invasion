#ifndef UTILS_H
#define UTILS_H

#include <time.h>

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

// return milliseconds
long get_ticks(void);

int LOG10(int v);

#endif // UTILS_H