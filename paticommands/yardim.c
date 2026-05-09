#include <stdio.h>
int main() {
    FILE *f = fopen("/etc/pati-help.txt", "r");
    if (f) {
        char satir[256];
        printf("\nKomutlar:\n");
    while (fgets(satir, sizeof(satir), f))
            printf("  %s", satir);
            printf("\n");
            fclose(f);
    } else {
        printf("Yardim dosyasi bulunamadi.\n");
            }
        }
