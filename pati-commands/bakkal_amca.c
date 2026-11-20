#include <sys/mount.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

int main() {
    if (mkdir("/proc", 0755) != 0 && errno != EEXIST) {
        perror("mkdir /proc");
    }
    mount("proc", "/proc", "proc", 0, NULL);

    if (mkdir("/sys", 0755) != 0 && errno != EEXIST) {
        perror("mkdir /sys");
    }
    mount("sysfs", "/sys", "sysfs", 0, NULL);

    if (mkdir("/dev", 0755) != 0 && errno != EEXIST) {
        perror("mkdir /dev");
    }
    mount("devtmpfs", "/dev", "devtmpfs", 0, NULL);

    char bootmode[16] = "normal";
    char *env = getenv("bootmode");
    if (env) {
        strncpy(bootmode, env, 15);
        bootmode[15] = 0;
    } else {
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

    if (mkdir("/newroot", 0755) != 0 && errno != EEXIST) perror("mkdir /newroot");
    if (mkdir("/newroot/dev", 0755) != 0 && errno != EEXIST) perror("mkdir /newroot/dev");
    if (mkdir("/newroot/proc", 0755) != 0 && errno != EEXIST) perror("mkdir /newroot/proc");
    if (mkdir("/newroot/sys", 0755) != 0 && errno != EEXIST) perror("mkdir /newroot/sys");
    if (mkdir("/newroot/oldroot", 0755) != 0 && errno != EEXIST) perror("mkdir /newroot/oldroot");

    int r;
    int squash_ok = 0;

    r = mount("/dev/vdb", "/newroot", "squashfs", MS_RDONLY, NULL);
    printf("squashfs mount /dev/vdb ret=%d errno=%d\n", r, errno);
    if (r == 0) {
        squash_ok = 1;
    } else {
        printf("[!] SquashFS is dead; trying ext4..\n");
    }

    if (squash_ok) {
        printf("[+] SquashFS is back.\n");
    } else {
        // fallback: ext4
        if (strcmp(bootmode, "recovery") == 0 || strcmp(bootmode, "fastboot") == 0)
            r = mount("/dev/vda6", "/newroot", "ext4", 0, NULL);
        else
            r = mount("/dev/vda5", "/newroot", "ext4", 0, NULL);
        printf("fallback ext4 mount root ret=%d errno=%d\n", r, errno);
        if (r != 0) {
            printf("[!] RootFS is dead, please halt.\n");
            for (;;) sleep(1);
        }
    }

    r = mount("/dev", "/newroot/dev", NULL, MS_MOVE, NULL);
    printf("mount /dev ret=%d errno=%d\n", r, errno);
    r = mount("/proc", "/newroot/proc", NULL, MS_MOVE, NULL);
    printf("mount /proc ret=%d errno=%d\n", r, errno);
    r = mount("/sys", "/newroot/sys", NULL, MS_MOVE, NULL);
    printf("mount /sys ret=%d errno=%d\n", r, errno);

    r = syscall(SYS_pivot_root, "/newroot", "/newroot/oldroot");
    printf("pivot_root ret=%d errno=%d\n", r, errno);
    if (r != 0) {
        printf("[!] pivot-root is dead\n");
        for (;;) sleep(1);
    }
    r = chdir("/");
    printf("chdir ret=%d errno=%d\n", r, errno);
    r = umount2("/oldroot", MNT_DETACH);
    printf("umount oldroot ret=%d errno=%d\n", r, errno);

    r = execv("/sbin/init", (char *[]){"/sbin/init", NULL});
    printf("execv /sbin/init failed! ret=%d errno=%d\n", r, errno);
    printf("access /sbin/init F_OK=%d X_OK=%d\n", access("/sbin/init", F_OK), access("/sbin/init", X_OK));
    struct stat st;
    r = stat("/sbin/init", &st);
    printf("stat /sbin/init ret=%d mode=0%o\n", r, st.st_mode);
    for (;;) sleep(1);
    return 0;
}
