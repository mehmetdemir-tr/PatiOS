#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/mount.h>

int main() {
    struct dirent *entry;
    pid_t pid;
    int status;
    struct dirent **namelist;
    int n = scandir("/dev/pcgconfigs", &namelist, NULL, alphasort);
    if (n < 0) {
        perror("[UYARI]: pcgconfigs acilamadi, sistem devam ediyor");
        while(wait(NULL) > 0);
        return 0;
    }
    printf("----------------------------------------\n");
    printf("----- MAUVYD Configuration Startup -----\n");
    printf("-------------Mounting FS..--------------\n");
    mount("proc", "/proc", "proc", 0, NULL);
    mount("sysfs", "/sys", "sysfs", 0, NULL);
    printf("-------------Listing Files..------------\n");
for (int i = 0; i < n; i++) {
    entry = namelist[i];
    if (strcmp(entry->d_name, ".") == 0) {
        continue;
    };
    if (strcmp(entry->d_name, "..") == 0) {
        continue;
    };
      usleep(10000);
      printf("Found: %s\n", entry->d_name);
      char tamyol[512];
      char dosya[256];
      char ofile[256];
      snprintf(tamyol, sizeof(tamyol), "/dev/pcgconfigs/%s", entry->d_name);
      FILE* pcgfile = fopen(tamyol, "r");
      if (pcgfile == NULL) {
        printf("Dosya bombos! atlaniyor..");
        continue;
        }
    char dosyayolu[256] = {0};
    char *args[] = {NULL, NULL};
    int bekle = 0;
    int izle = 0;
    while (fgets(dosya, 256, pcgfile) != NULL) {
            char kopya[256];
            strcpy(kopya, dosya);
            char *okuyucu = strtok(kopya, " =");
            if (okuyucu == NULL) continue;
            // char *okuyucu = strtok(dosya, " ="); 
            if (strcmp(okuyucu, "konumu") == 0) {
                char *gecici = strtok(NULL, " =");
                if (gecici != NULL) {
                    strncpy(dosyayolu, gecici, sizeof(dosyayolu)-1);
                    dosyayolu[sizeof(dosyayolu) - 1] = '\0';
                }
                args[0] = dosyayolu;
                dosyayolu[strcspn(dosyayolu, "\n")] = 0;
                printf("%s\n", dosyayolu);
            }

            if (strcmp(okuyucu, "bekle") == 0) {
                bekle = 1;
            }

            if (strcmp(okuyucu, "izle") == 0) {
                izle = 1;
            }
      };
      fclose(pcgfile);
      pid = fork(); // ÇATALLAMA ZAMANII!

    if (pid == -1) {
        perror("Catal kirildi :( (fork failed)");
        exit(EXIT_FAILURE);
        }
    if (pid > 0 && bekle == 1) {
        usleep(100000);
        wait(NULL);
        }

    if (pid == 0) {
        printf("Cocuk Islem > ben sunu baslatacagim: %s\n", dosyayolu);
        execv(dosyayolu, args);
        perror("YANIYOM ANAAMMM!");
        exit(EXIT_FAILURE);
        }
free(namelist[i]);
}
free(namelist);
while(wait(NULL) > 0);
// Invoking the programs
// Example: hello.ppg
// system("")
//.. böyle devam ediyor
}
