#include "pcg.h"

int pcg_read(const char *file_dst, const char *key, char *buf, size_t buf_sz) {
    FILE *f = fopen(file_dst, "r");
    if (!f) return -1;

    char line[512];
    while (fgets(line, sizeof(line), f)) {
        char *k = strtok(line, " =\n");
        if (!k) continue;
        if (strcmp(k, key) == 0) {
            char *v = strtok(NULL, " =\n");
            if (v) {
                strncpy(buf, v, buf_sz - 1);
                buf[buf_sz - 1] = '\0';
                fclose(f);
                return 0;
            }
        }
    }
    fclose(f);
    return -1;
}

int pcg_write(const char *file_dst, const char *key, const char *value) {
    FILE *f = fopen(file_dst, "r");
    if (!f) {
        f = fopen(file_dst, "w");
        if (!f) return -1;
        fprintf(f, "%s = %s\n", key, value);
        fclose(f);
        return 0;
    }

    char lines[64][512];
    int count = 0;
    int found = 0;

    while (fgets(lines[count], sizeof(lines[0]), f) && count < 64)
        count++;
    fclose(f);

    f = fopen(file_dst, "w");
    if (!f) return -1;

    for (int i = 0; i < count; i++) {
        char k[256];
        if (sscanf(lines[i], " %255s =", k) == 1 && strcmp(k, key) == 0) {
            fprintf(f, "%s = %s\n", key, value);
            found = 1;
        } else {
            fputs(lines[i], f);
        }
    }
    if (!found)
        fprintf(f, "%s = %s\n", key, value);

    fclose(f);
    return 0;
}
