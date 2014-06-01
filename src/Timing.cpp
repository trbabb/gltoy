#include "Timing.h"

#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))

#include <stdint.h>
#include <sys/time.h>

uint64_t local_epoch_start = 0;

double now() {
    timeval tv;
    gettimeofday(&tv, NULL);
    if (local_epoch_start == 0) local_epoch_start = tv.tv_sec;
    return tv.tv_sec - local_epoch_start + tv.tv_usec * 1e-6;
}

#else

#include <GLUT/glut.h>

double now() {
    return glutGet(GLUT_ELAPSED_TIME) / 1000.0;
}

#endif
