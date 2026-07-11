#ifndef PCG_H
#define PCG_H

#include <stdio.h>
#include <string.h>
#include <stddef.h>

int pcg_read(const char *file_dst, const char *key, char *buf, size_t buf_sz);
int pcg_write(const char *file_dst, const char *key, const char *value);

#endif
