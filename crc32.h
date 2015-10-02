 // daintree

#ifndef __CRC32_H__
#define __CRC32_H__

#include <arch.h>

typedef uint32_t crc32_t;

crc32_t crc32(void const *data, int n);

#endif

/* vim: set sw=4 et: */
