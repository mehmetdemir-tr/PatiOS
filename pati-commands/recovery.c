#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/reboot.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

void mount_all() {
    mkdir("/proc", 0755); mount("proc", "/proc", "proc", 0, NULL);
    mkdir("/sys", 0755);  mount("sysfs", "/sys", "sysfs", 0, NULL);
    mkdir("/dev", 0755);
    mount("devtmpfs", "/dev", "devtmpfs", 0, NULL);
    mkdir("/misc", 0755); mount("/dev/vda7", "/misc", "ext4", 0, NULL);
    mkdir("/cache", 0755); mount("/dev/vda9", "/cache", "ext4", 0, NULL);
    mkdir("/data", 0755); mount("/dev/vda1", "/data", "ext4", 0, NULL);
}

void write_bootmode(const char *mode) {
    int fd = open("/misc/bootmode", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) { write(fd, mode, strlen(mode)); close(fd); }
}

void reboot_system() {
    sync(); sync();
    reboot(RB_AUTOBOOT);
    for(;;) sleep(1);
}

void ota_apply() {
    printf("\n[OTA] Guncelleme kontrol ediliyor...\n");
    if (access("/cache/update.tar.gz", F_OK) != 0) {
        printf("[OTA] /cache/update.tar.gz bulunamadi.\n");
        printf("[OTA] Lutfen once guncelleme paketini /cache/ dizinine koyun.\n");
        return;
    }
    printf("[OTA] System partition guncelleniyor...\n");
    mkdir("/sysroot", 0755);
    mount("/dev/vda5", "/sysroot", "ext4", 0, NULL);
    setenv("PATH", "/sysroot/lib/paticommands:/sysroot/bin:/bin", 1);
    pid_t pid = fork();
    if (pid == 0) {
        execl("/lib/paticommands/tar", "tar", "-xzf", "/cache/update.tar.gz", "-C", "/sysroot", NULL);
        exit(1);
    }
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        printf("[OTA] Guncelleme basarili. Yeniden baslatiliyor...\n");
        write_bootmode("normal");
    } else {
        printf("[OTA] Guncelleme basarisiz!\n");
    }
    umount("/sysroot");
    rmdir("/sysroot");
}

void factory_reset() {
    printf("\n[SIFIRLA] Telefon sifirlaniyor...\n");
    printf("[SIFIRLA] /data partition'i temizleniyor...\n");
    umount("/data");
    pid_t pid = fork();
    if (pid == 0) {
        execl("/bin/mkfs", "mkfs.ext4", "-F", "/dev/vda1", NULL);
        exit(1);
    }
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        printf("[SIFIRLA] Basarili. Yeniden baslatiliyor...\n");
    } else {
        printf("[SIFIRLA] Basarisiz!\n");
    }
    mount("/dev/vda1", "/data", "ext4", 0, NULL);
}

void cli_shell() {
    printf("\n[CLI] Paket al ve yukle modu. Çıkmak icin 'çıkış' yazin.\n\n");
    mkdir("/sysroot", 0755);
    mount("/dev/vda5", "/sysroot", "ext4", 0, NULL);
    setenv("PATH", "/sysroot/lib/paticommands:/sysroot/bin:/bin", 1);
    pid_t pid = fork();
    if (pid == 0) {
        execl("/bin/shell", "shell", NULL);
        exit(1);
    }
    int status;
    waitpid(pid, &status, 0);
    umount("/sysroot");
}

void fastboot_mode() {
    printf("\n[FASTBOOT] Fastboot moduna geciliyor...\n");
    write_bootmode("fastboot");
    printf("[FASTBOOT] Yeniden baslatiliyor...\n");
    reboot_system();
}

void show_menu() {
    printf("\n");printf("\n");
    printf("========== PatiOS Kurtarma Bolumu ==========\n");
    printf("  1. OTA (Over The Air) Guncellemesi yap\n");
    printf("  2. Telefonu Sifirla\n");
    printf("  3. CLI'dan paket al ve yukle\n");
    printf("  4. Yeniden baslat\n");
    printf("  5. Fastboot moduna gir\n");
    printf("============================================\n");
    printf("Seciminiz >> ");
    fflush(stdout);
}

int main() {
    mount_all();
    printf("\nPatiOS Recovery v1.0\n");
    while (1) {
        show_menu();
        char buf[16];
        if (!fgets(buf, sizeof(buf), stdin)) continue;
        switch (buf[0]) {
            case '1': ota_apply(); break;
            case '2': factory_reset(); break;
            case '3': cli_shell(); break;
            case '4':
                write_bootmode("normal");
                printf("Yeniden baslatiliyor...\n");
                reboot_system();
                break;
            case '5': fastboot_mode(); break;
            default:  printf("Gecersiz secim.\n"); break;
        }
    }
    return 0;
}
