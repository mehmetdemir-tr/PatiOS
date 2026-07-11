#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <zlib.h>

#define TAR_BLOCK_SIZE 512

struct tar_header {
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
    char padding[12];
} __attribute__((packed));

static int is_zero(const char *buf, int len) {
    for (int i = 0; i < len; i++)
        if (buf[i]) return 0;
    return 1;
}

static unsigned long from_octal(const char *str, int len) {
    unsigned long val = 0;
    for (int i = 0; i < len && str[i] && str[i] != ' '; i++)
        val = val * 8 + (str[i] - '0');
    return val;
}

static int make_parent_dirs(const char *path) {
    char tmp[1024];
    snprintf(tmp, sizeof(tmp), "%s", path);
    for (char *p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            mkdir(tmp, 0755);
            *p = '/';
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    const char *archive = NULL;
    const char *dest = ".";
    int gz = 0;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            int j = 1;
            while (argv[i][j]) {
                switch (argv[i][j]) {
                    case 'x': break;
                    case 'z': gz = 1; break;
                    case 'C':
                        if (argv[i][j+1]) {
                            dest = argv[i] + j + 1;
                            goto next_arg;
                        } else if (i + 1 < argc) {
                            dest = argv[++i];
                            goto next_arg;
                        }
                        break;
                }
                j++;
            }
        } else {
            archive = argv[i];
        }
        next_arg:;
    }

    if (!archive) {
        fprintf(stderr, "Kullanimi: tar -xz -C <hedef> <arsiv.tar.gz>\n");
        return 1;
    }

    gzFile gzf = gzopen(archive, "rb");
    if (!gzf) {
        fprintf(stderr, "HATA: %s acilamadi.\n", archive);
        return 1;
    }

    int ok = 1;
    while (1) {
        struct tar_header h;
        int n = gzread(gzf, &h, TAR_BLOCK_SIZE);
        if (n < TAR_BLOCK_SIZE) break;
        if (is_zero((char *)&h, TAR_BLOCK_SIZE)) break;

        char fullpath[1024];
        if (h.prefix[0])
            snprintf(fullpath, sizeof(fullpath), "%s/%s/%s", dest, h.prefix, h.name);
        else
            snprintf(fullpath, sizeof(fullpath), "%s/%s", dest, h.name);

        unsigned long filesize = from_octal(h.size, 12);
        unsigned long blocks = (filesize + TAR_BLOCK_SIZE - 1) / TAR_BLOCK_SIZE;

        if (h.name[0] == '/' || strstr(h.name, "../")) {
            fprintf(stderr, "Guvenlik: atlaniyor (mutlak/ust dizin yolu): %s\n", h.name);
            gzseek(gzf, blocks * TAR_BLOCK_SIZE, SEEK_CUR);
            continue;
        }

        switch (h.typeflag) {
            case '5':
                make_parent_dirs(fullpath);
                mkdir(fullpath, 0755);
                break;

            case '0':
            case '\0':
                make_parent_dirs(fullpath);
                {
                    int fd = open(fullpath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd < 0) {
                        fprintf(stderr, "HATA: %s olusturulamadi\n", fullpath);
                        ok = 0;
                        gzseek(gzf, blocks * TAR_BLOCK_SIZE, SEEK_CUR);
                        break;
                    }
                    char buf[TAR_BLOCK_SIZE];
                    unsigned long remaining = filesize;
                    while (remaining > 0) {
                        int chunk = remaining < sizeof(buf) ? remaining : sizeof(buf);
                        int r = gzread(gzf, buf, chunk);
                        if (r <= 0) break;
                        write(fd, buf, r);
                        remaining -= r;
                    }
                    close(fd);
                }
                break;

            case '2':
                make_parent_dirs(fullpath);
                symlink(h.linkname, fullpath);
                break;

            default:
                gzseek(gzf, blocks * TAR_BLOCK_SIZE, SEEK_CUR);
                break;
        }
    }

    gzclose(gzf);
    return ok ? 0 : 1;
}
