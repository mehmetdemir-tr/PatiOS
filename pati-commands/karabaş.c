#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <libgen.h>

void karabas() { // <-- güncel olmayan kod
    struct dirent **namelist;
    int n = scandir("/dev/pcgconfigs", &namelist, NULL, alphasort);
    if (n < 0) {
        printf("pcgconfigs açılamadı!\n");
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
            strncpy(kopya, dosya, sizeof(kopya) - 1);
            kopya[sizeof(kopya)-1] = '\0';
            char *okuyucu = strtok(kopya, " =\n");
            if (okuyucu == NULL) continue;
            if (strcmp(okuyucu, "konumu") == 0) {
                char *gecici = strtok(NULL, " =\n");
                if (gecici != NULL) strncpy(dosyayolu, gecici, sizeof(dosyayolu) - 1);
                dosyayolu[sizeof(dosyayolu)-1] = '\0';
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
                        strncpy(adkopya, dosyayolu, sizeof(adkopya) - 1);
                        adkopya[sizeof(adkopya)-1] = '\0';
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
            strncpy(adkopya, dosyayolu, sizeof(adkopya) - 1);
            adkopya[sizeof(adkopya)-1] = '\0';
            printf("%-20s [çalışmıyor]\n", basename(adkopya));
        }
        free(namelist[i]);
    }
    free(namelist);
}
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
        ps_tumu();
    } else {
        ps_tumu();
    }
    return 0;
}
