#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <libgen.h>

void ps_tumu() {
    struct dirent **proclist;
    int np = scandir("/proc", &proclist, NULL, alphasort);
    if (np < 0) { printf("proc okunamadi\n"); return; }

    printf("%-20s %s\n", "Programlar", "PID");
    printf("----------------------------------\n");
    for (int j = 0; j < np; j++) {
        if (atoi(proclist[j]->d_name) > 0) {
            char yol[512];
            snprintf(yol, sizeof(yol), "/proc/%s/cmdline", proclist[j]->d_name);
            FILE *f = fopen(yol, "r");
            if (f) {
                char cmd[256] = {0};
                fread(cmd, 1, sizeof(cmd) - 1, f);
                fclose(f);
                if (cmd[0]) {
                    char *ad = strrchr(cmd, '/');
                    if (ad) ad++; else ad = cmd;
                    printf("%-20s PID: %s\n", ad, proclist[j]->d_name);
                }
            }
        }
        free(proclist[j]);
    }
    free(proclist);
}

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "-a") == 0) {
        printf("karabaş -a kaldırıldı, direk karabaş yazarak bütün processleri görüntüleyebilirsin. :)");
    } else {
        ps_tumu();
    }
    return 0;
}
