#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

static int mkdir_p(const char *path) {
    char tmp[256];
    snprintf(tmp, sizeof(tmp), "%s", path);
    size_t len = strlen(tmp);
    if (len == 0) return 0;
    if (tmp[len - 1] == '/') tmp[len - 1] = '\0';
    for (char *p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            if (mkdir(tmp, 0755) < 0 && errno != EEXIST) {
                perror("mkdir başarısız");
                return -1;
            }
            *p = '/';
        }
    }
    if (mkdir(tmp, 0755) < 0 && errno != EEXIST) {
        perror("mkdir başarısız");
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("kullanım: mkdir [-p] <dizin>...\n");
        return 1;
    }
    int parents = 0, i = 1;
    if (strcmp(argv[1], "-p") == 0) {
        parents = 1;
        i = 2;
    }
    if (i >= argc) {
        printf("kullanım: mkdir [-p] <dizin>...\n");
        return 1;
    }
    for (; i < argc; i++) {
        if (parents) {
            if (mkdir_p(argv[i]) < 0) return 1;
        } else {
            if (mkdir(argv[i], 0755) < 0) {
                perror("mkdir başarısız");
                return 1;
            }
        }
    }
    return 0;
}