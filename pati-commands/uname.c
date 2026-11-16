#include <stdio.h>
#include <sys/utsname.h>
#include <string.h>

int main(int argc, char *argv[]) {
    struct utsname u;
    if (uname(&u) < 0) {
        perror("uname");
        return 1;
    }
    int all = 0, s = 0, n = 0, r = 0, v = 0, m = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--all") == 0) all = 1;
        else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--kernel-name") == 0) s = 1;
        else if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--nodename") == 0) n = 1;
        else if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--kernel-release") == 0) r = 1;
        else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--kernel-version") == 0) v = 1;
        else if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--machine") == 0) m = 1;
    }
    if (all || !(s || n || r || v || m)) {
        s = n = r = v = m = 1;
    }
    if (s) printf("%s ", u.sysname);
    if (n) printf("%s ", u.nodename);
    if (r) printf("%s ", u.release);
    if (v) printf("%s ", u.version);
    if (m) printf("%s ", u.machine);
    printf("\n");
    return 0;
}