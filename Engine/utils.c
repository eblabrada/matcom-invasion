#include "utils.h"

int LOG10(int v) {
    int ret = 0;
    while (v > 0)
        ret++, v /= 10;
    return ret;
}

long get_ticks(void) {
    struct timespec ts;
    clock_gettime(1, &ts);
    return (long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}