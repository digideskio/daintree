 // daintree

#include <crc32.h>

crc32_t crc32(void const *data, int n) {
    crc32_t r = 0xFFFFFFFF;

    for (int i = 0; i < n; ++i) {
        r ^= ((char const *)data)[i];

        /** \note Loop unrolled for perf. */
        if ((r & 0x1) != 0) { r = (r >> 1) ^ 0xEDB88320; } else { r = r >> 1; }
        if ((r & 0x1) != 0) { r = (r >> 1) ^ 0xEDB88320; } else { r = r >> 1; }
        if ((r & 0x1) != 0) { r = (r >> 1) ^ 0xEDB88320; } else { r = r >> 1; }
        if ((r & 0x1) != 0) { r = (r >> 1) ^ 0xEDB88320; } else { r = r >> 1; }
        if ((r & 0x1) != 0) { r = (r >> 1) ^ 0xEDB88320; } else { r = r >> 1; }
        if ((r & 0x1) != 0) { r = (r >> 1) ^ 0xEDB88320; } else { r = r >> 1; }
        if ((r & 0x1) != 0) { r = (r >> 1) ^ 0xEDB88320; } else { r = r >> 1; }
        if ((r & 0x1) != 0) { r = (r >> 1) ^ 0xEDB88320; } else { r = r >> 1; }
    }

    return r ^ 0xFFFFFFFF;
}

/* vim: set sw=4 et: */
