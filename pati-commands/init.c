#include <sys/mount.h>
#include <sys/syscall.h>
#include <unistd.h>

int main() {
    mkdir("/mnt", 0755);
    mount("/dev/vda5", "/mnt", "ext4", 0, NULL);
    mount("/dev", "/mnt/dev", NULL, MS_MOVE, NULL);
    mount("/proc", "/mnt/proc", NULL, MS_MOVE, NULL);
    mount("/sys", "/mnt/sys", NULL, MS_MOVE, NULL);
    syscall(SYS_pivot_root, "/mnt", "/mnt/oldroot");
    chdir("/");
    umount2("/oldroot", MNT_DETACH);
    execv("/sbin/init", (char *[]){"/sbin/init", NULL});
    return 0;
}