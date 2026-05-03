#ifndef PCG_H
#define PCG_H

#include <stdio.h>
#include <string.h>
#include <stddef.h>

int pcg_read(const char *dosya_yolu, const char *key, char *buf, size_t buf_sz);

#endif
