#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <libgen.h>

void karabas() {
    struct dirent **namelist;
    int n = scandir("/dev/pcgconfigs", &namelist, NULL, alphasort);
    if (n < 0) {
        printf("pcgconfigs acilamadi!\n");
        return;
    }

    printf("%-20s %s\n", "Programlar", "PID");
    printf("----------------------------------\n");

    for (int i = 0; i < n; i++) {
        if (strcmp(namelist[i]->d_name, ".") == 0) { free(namelist[i]); continue; }
        if (strcmp(namelist[i]->d_name, "..") == 0) { free(namelist[i]); continue; }
        if (strstr(namelist[i]->d_name, ".pcg") == NULL) { free(namelist[i]); continue; }

        char tamyol[512];
        char dosya[256];
        snprintf(tamyol, sizeof(tamyol), "/dev/pcgconfigs/%s", namelist[i]->d_name);
        FILE *pcgfile = fopen(tamyol, "r");
        if (pcgfile == NULL) { free(namelist[i]); continue; }

        char dosyayolu[256] = {0};
        while (fgets(dosya, sizeof(dosya), pcgfile) != NULL) {
            char kopya[256];
            strcpy(kopya, dosya);
            char *okuyucu = strtok(kopya, " =\n");
            if (okuyucu == NULL) continue;
            if (strcmp(okuyucu, "konumu") == 0) {
                char *gecici = strtok(NULL, " =\n");
                if (gecici != NULL) strcpy(dosyayolu, gecici);
            }
        }
        fclose(pcgfile);

        if (strlen(dosyayolu) == 0) { free(namelist[i]); continue; }

        struct dirent **proclist;
        int np = scandir("/proc", &proclist, NULL, alphasort);
        if (np < 0) { free(namelist[i]); continue; }

        int bulundu = 0;
        for (int j = 0; j < np; j++) {
            if (atoi(proclist[j]->d_name) > 0) {
                snprintf(tamyol, sizeof(tamyol), "/proc/%s/cmdline", proclist[j]->d_name);
                FILE *cmdfile = fopen(tamyol, "r");
                if (cmdfile != NULL) {
                    char cmdline[256] = {0};
                    fread(cmdline, 1, sizeof(cmdline) - 1, cmdfile);
                    fclose(cmdfile);
                    if (strcmp(cmdline, dosyayolu) == 0) {
                        char adkopya[256];
                        strcpy(adkopya, dosyayolu);
                        printf("%-20s PID: %s\n", basename(adkopya), proclist[j]->d_name);
                        bulundu = 1;
                    }
                }
            }
            free(proclist[j]);
        }
        free(proclist);

        if (bulundu == 0) {
            char adkopya[256];
            strcpy(adkopya, dosyayolu);
            printf("%-20s [calismiyor]\n", basename(adkopya));
        }
        free(namelist[i]);
    }
    free(namelist);
}

int main(void) {
    karabas();
    return 0;
}
