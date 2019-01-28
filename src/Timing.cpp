#include "Timing.h"

#if false && (defined (__unix__) || (defined (__APPLE__) && defined (__MACH__)))

// this stuff, I think, was intended to get a higher resolution timer.
// now that we are on glfw, I think it's probably unnecessary

#include <stdint.h>
#include <cstddef>
#include <sys/time.h>

uint64_t local_epoch_start = 0;

double now() {
    timeval tv;
    gettimeofday(&tv, NULL);
    if (local_epoch_start == 0) local_epoch_start = tv.tv_sec;
    return tv.tv_sec - local_epoch_start + tv.tv_usec * 1e-6;
}

#else

#include "glHeaders.h"

double now() {
    return glfwGetTime();
}

#endif
