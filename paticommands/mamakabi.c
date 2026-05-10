#include <stdio.h>
#include <string.h>

int main() {
    FILE *f = fopen("/proc/meminfo", "r");
    if (!f) { printf("MAMAM YOK CALDİLAR BANA MAMA VERMEYE NE DERSİN? D:\n"); return 1; }

    char satir[256];
    unsigned long toplam = 0, kullanilabilir = 0, bos = 0, takas_toplam = 0, takas_bos = 0;
        while (fgets(satir, sizeof(satir), f)) {
        unsigned long val;
        if      (sscanf(satir, "MemTotal: %lu kB", &val) == 1) toplam = val;
        else if (sscanf(satir, "MemAvailable: %lu kB", &val) == 1) kullanilabilir = val;
        else if (sscanf(satir, "MemFree: %lu kB", &val) == 1) bos = val;
        else if (sscanf(satir, "SwapTotal: %lu kB", &val) == 1) takas_toplam = val;
        else if (sscanf(satir, "SwapFree: %lu kB", &val) == 1) takas_bos = val;
    }
    fclose(f);

    unsigned long kullanilan = toplam - kullanilabilir;
    int yuzde = toplam > 0 ? (kullanilan * 100 / toplam) : 0;

    printf("MAMA KABI -- Bellek Durumu\n");
    printf("---------------------------------\n");
    printf("Toplam  : %5lu MB\n", toplam / 1024);
    printf("Kullanim: %5lu MB (%d%%)\n", kullanilan / 1024, yuzde);
    printf("Bos     : %5lu MB\n", kullanilabilir / 1024);
    if (takas_toplam > 0)
        printf("Takas   : %5lu MB\n", takas_toplam / 1024);

    return 0;
}

