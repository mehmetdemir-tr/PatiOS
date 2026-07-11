#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

void yardim() {
    printf("Kullanim: imeiflasher <komut> [arguman]\n");
    printf("  imeiflasher goster              - Mevcut IMEI bilgisi\n");
    printf("  imeiflasher yedek              - current'i backup'a kopyala\n");
    printf("  imeiflasher gerial             - backup'tan current'a geri yukle\n");
    printf("  imeiflasher yaz <imei1> [imei2] - Yeni IMEI yaz (current)\n");
}

int dosya_oku(const char *yol, char *buf, size_t sz) {
    FILE *f = fopen(yol, "r");
    if (!f) return -1;
    fgets(buf, sz, f);
    char *nl = strchr(buf, '\n');
    if (nl) *nl = 0;
    fclose(f);
    return 0;
}

int dosya_yaz(const char *yol, const char *icerik) {
    FILE *f = fopen(yol, "w");
    if (!f) return -1;
    fprintf(f, "%s\n", icerik);
    fclose(f);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) { yardim(); return 1; }

    if (strcmp(argv[1], "goster") == 0) {
        char imei1[32], imei2[32], imeibackup[32], imeibackup2[32];
        printf("==== Cihaz Bilgileri ====\n");
        if (dosya_oku("/dev/imeidata/efs_current/imei.txt", imei1, sizeof(imei1)) == 0)
            printf("Aktif IMEI 1: %s\n", imei1);
        if (dosya_oku("/dev/imeidata/efs_current/imei2.txt", imei2, sizeof(imei2)) == 0)
            printf("Aktif IMEI 2: %s\n", imei2);
        if (dosya_oku("/dev/imeidata/efs_backup/imei.txt", imeibackup, sizeof(imeibackup)) == 0)
            printf("Yedek IMEI 1: %s\n", imeibackup);
        if (dosya_oku("/dev/imeidata/efs_backup/imei2.txt", imeibackup2, sizeof(imeibackup2)) == 0)
            printf("Yedek IMEI 2: %s\n", imeibackup2);

        if (imei1[0] && imeibackup[0] && strcmp(imei1, imeibackup) == 0)
            printf("Durum: Orijinal IMEI kullanimda (backup ile uyumlu)\n");
        else if (imei1[0] && imeibackup[0])
            printf("UYARI: Aktif IMEI yedekten farkli!\n");
        return 0;
    }

    if (strcmp(argv[1], "yedek") == 0) {
        printf("EFS yaziliyor: current -> backup...\n");
        mount("/dev/vda3", "/dev/imeidata/efs_backup", "ext4", MS_REMOUNT, NULL);
        int in1 = open("/dev/imeidata/efs_current/imei.txt", O_RDONLY);
        if (in1 < 0) { perror("current imei.txt okunamadi"); return 1; }
        int out1 = open("/dev/imeidata/efs_backup/imei.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        char buf[4096]; ssize_t n;
        while ((n = read(in1, buf, sizeof(buf))) > 0) write(out1, buf, n);
        close(in1); close(out1);

        int in2 = open("/dev/imeidata/efs_current/imei2.txt", O_RDONLY);
        if (in2 >= 0) {
            int out2 = open("/dev/imeidata/efs_backup/imei2.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
            while ((n = read(in2, buf, sizeof(buf))) > 0) write(out2, buf, n);
            close(in2); close(out2);
        }
        mount("/dev/vda3", "/dev/imeidata/efs_backup", "ext4", MS_RDONLY, NULL);
        printf("[TAMAM]: Yedekleme tamam.\n");
        return 0;
    }

    if (strcmp(argv[1], "gerial") == 0) {
        printf("Geri yukleniyor: backup -> current...\n");
        mount("/dev/vda3", "/dev/imeidata/efs_backup", "ext4", MS_REMOUNT, NULL);
        int in1 = open("/dev/imeidata/efs_backup/imei.txt", O_RDONLY);
        if (in1 < 0) { perror("backup imei.txt okunamadı"); return 1; }
        int out1 = open("/dev/imeidata/efs_current/imei.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        char buf[4096]; ssize_t n;
        while ((n = read(in1, buf, sizeof(buf))) > 0) write(out1, buf, n);
        close(in1); close(out1);

        int in2 = open("/dev/imeidata/efs_backup/imei2.txt", O_RDONLY);
        if (in2 >= 0) {
            int out2 = open("/dev/imeidata/efs_current/imei2.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
            while ((n = read(in2, buf, sizeof(buf))) > 0) write(out2, buf, n);
            close(in2); close(out2);
        }
        printf("[TAMAM]: Geri yükleme tamamlandı.\n");
        return 0;
    }

    if (strcmp(argv[1], "yaz") == 0) {
        if (argc < 3) { printf("IMEI numarasi gerekli\n"); return 1; }
        printf("UYARI: IMEI degistiriliyor! Bu yasal sonuclar dogurabilir.\n");
        sleep(2);
        dosya_yaz("/dev/imeidata/efs_current/imei.txt", argv[2]);
        if (argc > 3)
            dosya_yaz("/dev/imeidata/efs_current/imei2.txt", argv[3]);
        printf("[TAMAM]: IMEI yazildi.\n");
        return 0;
    }

    yardim();
    return 1;
}