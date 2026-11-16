#include <sys/mount.h>
#include <sys/syscall.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

int main() {
    mkdir("/proc", 0755);
    mount("proc", "/proc", "proc", 0, NULL);

    char bootmode[16] = "normal";

    char *env = getenv("bootmode");
    if (env) {
        strncpy(bootmode, env, 15);
        bootmode[15] = 0;
    } else {
        /* fallback: read it from the kernel cmdline */
        mkdir("/proc", 0755);
        mount("proc", "/proc", "proc", 0, NULL);
        int fd = open("/proc/cmdline", O_RDONLY);
        if (fd >= 0) {
            char cmdline[512] = {0};
            int n = read(fd, cmdline, sizeof(cmdline) - 1);
            close(fd);
            if (n > 0) {
                cmdline[n] = 0;
                char *p = strstr(cmdline, "bootmode=");
                if (p) {
                    p += 9;
                    int i = 0;
                    while (*p && *p != ' ' && i < 15) bootmode[i++] = *p++;
                    bootmode[i] = 0;
                }
            }
        }
    }

    mkdir("/newroot", 0755);
    mkdir("/newroot/dev", 0755);
    mkdir("/newroot/proc", 0755);
    mkdir("/newroot/sys", 0755);

    int r;
    if (strcmp(bootmode, "recovery") == 0 || strcmp(bootmode, "fastboot") == 0)
        r = mount("/dev/vda6", "/newroot", "ext4", 0, NULL);
    else
        r = mount("/dev/vda5", "/newroot", "ext4", 0, NULL);
    printf("mount root ret=%d errno=%d\n", r, errno);

    mkdir("/newroot/oldroot", 0755);
    r = mount("/dev", "/newroot/dev", NULL, MS_MOVE, NULL);
    printf("mount /dev ret=%d errno=%d\n", r, errno);
    r = mount("/proc", "/newroot/proc", NULL, MS_MOVE, NULL);
    printf("mount /proc ret=%d errno=%d\n", r, errno);
    r = mount("/sys", "/newroot/sys", NULL, MS_MOVE, NULL);
    printf("mount /sys ret=%d errno=%d\n", r, errno);
    r = syscall(SYS_pivot_root, "/newroot", "/newroot/oldroot");
    printf("pivot_root ret=%d errno=%d\n", r, errno);
    r = chdir("/");
    printf("chdir ret=%d errno=%d\n", r, errno);
    r = umount2("/oldroot", MNT_DETACH);
    printf("umount oldroot ret=%d errno=%d\n", r, errno);
    struct stat lst;
    r = stat("/sbin", &lst);
    printf("stat /sbin ret=%d mode=0%o\n", r, lst.st_mode);
    r = stat("/", &lst);
    printf("stat / ret=%d ino=%lu\n", r, lst.st_ino);
    r = execv("/sbin/init", (char *[]){"/sbin/init", NULL});
    printf("execv /sbin/init failed! ret=%d errno=%d\n", r, errno);
    printf("access /sbin/init F_OK=%d X_OK=%d\n", access("/sbin/init", F_OK), access("/sbin/init", X_OK));
    struct stat st;
    r = stat("/sbin/init", &st);
    printf("stat /sbin/init ret=%d mode=0%o\n", r, st.st_mode);
    for (;;) sleep(1);
}
