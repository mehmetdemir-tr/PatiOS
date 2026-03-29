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
    while (fgets(dosya, 256, pcgfile) != NULL) {
            char kopya[256];
            strcpy(kopya, dosya);
            char *okuyucu = strtok(kopya, " =");
            // char *okuyucu = strtok(dosya, " ="); 
            if (strcmp(okuyucu, "konumu") == 0) {
                char *gecici = strtok(NULL, " =");
                strcpy(dosyayolu, gecici);
                args[0] = dosyayolu;
                dosyayolu[strcspn(dosyayolu, "\n")] = 0;
                printf("%s\n", dosyayolu);
            }

            if (strcmp(okuyucu, "bekle") == 0) {
                bekle = 1;
            }
      };
      pid = fork(); // ÇATALLAMA ZAMANII! (Şaka amaçlı yorum satırı, silmeyin!1)

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
