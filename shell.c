#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <dirent.h>
#include <sys/reboot.h>


int main(void) {
while (1) {
    printf("pati@tr> ");
    fflush(stdout);
    char girdi[256];
    fgets(girdi, 256, stdin);
    girdi[strcspn(girdi, "\n")] = 0;
    char *komut = strtok(girdi, " ");
    if (komut == NULL) {
        continue;
    }
    char *arguman = strtok(NULL, " ");
    if (strcmp(komut, "pati") == 0) {
        printf("Miyavv!");
        printf("\n");
    };
    if (strcmp(komut, "temizle") == 0) {
       for (int i = 0; i < 50; i++) {
        printf("\n");
       }
    };
    if (strcmp(komut, "ls") == 0) {
        struct dirent *entry;
        DIR* dirFile = opendir( "." );
        if (arguman == NULL) {
            dirFile = opendir( "." );
        }
        else {
           dirFile = opendir(arguman);
        }
        if (dirFile == NULL) {
            perror("Dizin yok veya acilamadi!1!1");
            return 1;
        }

        while ((entry = readdir(dirFile)) != NULL) {
            printf("%-20s\t", entry->d_name);
        }
        closedir(dirFile);
        printf("\n");
    };
    if (strcmp(komut, "uname") == 0) {
        printf("Pati-0.1 by Mehmet Demir. Kod adi:Cilek (Strawberry)");
        printf("\n");
    };
    if (strcmp(komut, "cikis") == 0) {
        reboot(RB_POWER_OFF);
    };
    if (strcmp(komut, "cat") == 0) {
        char girdi1[256];
        if (arguman == NULL) {
            continue;
        }
        FILE* ofile = fopen(arguman, "r");
        if (ofile == NULL) {
            printf("Acmaya calistigin dosya bombos? Kalbin gibi :)\n");
            continue;
        }

        while (fgets(girdi1, 256, ofile) != NULL) {
            printf("%s", girdi1);
        }
        fclose(ofile);
        printf("\n");
    };

    if (strcmp(komut, "mamakabi") == 0) {
        char girdi1[256];
        FILE* ofile = fopen("/proc/meminfo","r");

        if (ofile == NULL) {
            printf("MAMAM YOK CALDİLAR D:\n");
            continue;
        }

        while (fgets(girdi1, 256, ofile) != NULL) {
            printf("%s", girdi1);
        }
        fclose(ofile);
        printf("\n");
    };

    if (strcmp(komut, "yardim") == 0) {
        printf("\nKomutlar;\npati = Supriz komutu\nuname = Sistem bilgisi\ncat = dosya okur\ntemizle = ekrani temizler\nls = klasorleri listeler\nmamakabi = RAM bilgisi\n");
    };
    }
    }
