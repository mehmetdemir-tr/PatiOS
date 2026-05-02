#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <dirent.h>
#include <stdlib.h>
#include <libgen.h>
#include <sys/reboot.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

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
            printf("Miyavv!\n");
        }
        else if (strcmp(komut, "miyav") == 0) {
            if (arguman == NULL) {
                printf("Kullanim: miyav <adres>\n");
                continue;
            }

            struct addrinfo *sonuc;
            int ret = getaddrinfo(arguman, NULL, NULL, &sonuc);
            if (ret != 0) {
                printf("miyav: adres cozumlenemedi: %s\n", arguman);
                continue;
            }

            int soket = socket(AF_INET, SOCK_STREAM, 0);
            if (soket < 0) {
                printf("miyav: soket acilamadi\n");
                freeaddrinfo(sonuc);
                continue;
            }
            struct timeval timeout;
            timeout.tv_sec = 2;
            timeout.tv_usec = 0;
            setsockopt(soket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

            struct sockaddr_in hedef;
            hedef.sin_family = AF_INET;
            hedef.sin_port = htons(53);
            hedef.sin_addr = ((struct sockaddr_in *)sonuc->ai_addr)->sin_addr;

            if (connect(soket, (struct sockaddr *)&hedef, sizeof(hedef)) == 0) {
                printf("miyav: %s ulasilabilir, kedy mutlu! :)\n", arguman);
            } else {
                printf("miyav: %s cevap vermiyor... kedy uzgun :(\n", arguman);
            }

            close(soket);
            freeaddrinfo(sonuc);
        }
        else if (strcmp(komut, "temizle") == 0) {
            for (int i = 0; i < 50; i++) {
                printf("\n");
            }
            printf("Ekran Gumletildi!\n");
        }
        else if (strcmp(komut, "ls") == 0) {
            struct dirent *entry;
            DIR *dirFile;
            if (arguman == NULL) {
                dirFile = opendir(".");
            } else {
                dirFile = opendir(arguman);
            }
            if (dirFile == NULL) {
                perror("Dizin yok veya acilamadi");
                continue;
            }
            while ((entry = readdir(dirFile)) != NULL) {
                printf("%-20s\t", entry->d_name);
            }
            closedir(dirFile);
            printf("\n");
        }
        else if (strcmp(komut, "uname") == 0) {
            printf("Pati-1.0 by Mehmet Demir. Kod adi: Cilek (Strawberry)\n");
        }
        else if (strcmp(komut, "touch") == 0) {
            if (arguman == NULL) {
                printf("Kullanim: touch <dosya_adi>\n");
                continue;
            }
            int fd = creat(arguman, 0644);
            if (fd != -1) {
                printf("Dosya olusturuldu: %s\n", arguman);
                close(fd);
                sync();
            } else {
                perror("touch hatasi");
            }
        }
        else if (strcmp(komut, "cikis") == 0) {
            sync();
            reboot(RB_POWER_OFF);
        }
        else if (strcmp(komut, "cat") == 0) {
            char girdi1[256];
            if (arguman == NULL) {
                printf("Kullanim: cat <dosya>\n");
                continue;
            }
            FILE *ofile = fopen(arguman, "r");
            if (ofile == NULL) {
                printf("Acmaya calistigin dosya bombos? Kalbin gibi :)\n");
                continue;
            }
            while (fgets(girdi1, 256, ofile) != NULL) {
                printf("%s", girdi1);
            }
            fclose(ofile);
            printf("\n");
        }
        else if (strcmp(komut, "acil-durum") == 0) {
            printf("\n\n");
            printf("                                                 Acil Yeniden Baslatma Protokolu\n");
            printf("                                        Bu komut/buton'u sadece acil durumlarda\n");
            printf("                                 kullanin, gerekmedikce kullanmaniz sistemi bozabilir.\n");
            printf("                                                  Devam Etmek Istiyormusunuz? (E/H): ");
            fflush(stdout);
            char onay[10];
            fgets(onay, 10, stdin);
            if (onay[0] == 'E' || onay[0] == 'e') {
                printf("Sistem yeniden baslatiliyor...\n");
                reboot(RB_AUTOBOOT);
            } else {
                printf("Acil yeniden baslatma iptal edildi.\n");
        }
    }
        else if (strcmp(komut, "mamakabi") == 0) {
            char girdi1[256];
            FILE *ofile = fopen("/proc/meminfo", "r");
            if (ofile == NULL) {
                printf("MAMAM YOK CALDILAR D:\n");
                continue;
            }
            while (fgets(girdi1, 256, ofile) != NULL) {
                printf("%s", girdi1);
            }
            fclose(ofile);
            printf("\n");
        }
        else if (strcmp(komut, "libreturks") == 0) {
            printf("########=--:--=+=---:::::::--:-::::::::::::::::::::::::--:::::::::::--:---\n");
            printf("########=--::-=++---------------------::::::::::::::::::::::::::::::----:::\n");
            printf("########+--::--+=-----------------------::-::::::::::::::::::::::::::::::::\n");
            printf("########+-::::-==-----=-=====-------==---=--------:::::::::::::::::::::::::\n");
            printf("########+-::::-==-------====--========++++++==+====---------:::::::::::::::\n");
            printf("########*-::::-=--------=----:-=######%%%%#*+========+++++++++===--::::::::::::::\n");
            printf("########*-:::--------==--:::::---=+=====--=====++=++**##*++==---:::::::::::\n");
            printf("########*-:::-----==++*+++++++++++++======+++++++**++===+**+++*+++=-:::::::\n");
            printf("########*=:----=++*++=++****++++++++++++++++++++++++**##**++++++++++++--:::\n");
            printf("########*=-===++++++=+++*********+++++++++**************+++===+=++++++++--:\n");
            printf("########+==+*++*+++++*************+***+*++++++******#***+**+++++++++++**=-:\n");
            printf("########*+*************#*********+******+++******########***+==+++++**+*=--\n");
            printf("#########+==---***#####********************#****##########***+++*****+++---\n");
            printf("#########+-:::-=*####*#*#***#***#**********##*+****########******#**+++=---\n");
            printf("******++++++++++*####***#*+*#*++**+++=*++++##*+**###*****####*******+=+----\n");
            printf("*********++++++*******+***++#*++++===-=+=++**++**********######**##*+=+++++\n");
            printf("#*******+++++=+++****++***++*+++=---::--==+**+++*****++*************+++++++\n");
            printf("%%%%%%####+=++**###%%%%#%%%%@@%%%%*+=--:::::---==+###%%%%#*#***#*******+++++=+++++\n");
            printf("@@@@@@@@%%%%#=++*******#*@@@@@#+----:::::---=#*@%%#%%#*##%%%%#*****+*+++==***#*\n");
            printf("@@@@@@@@@@%%+=++*****+++##@@%%@#+---::::::::-+#+%%@@@@#******#***++++++==*####\n");
            printf("@@@@@@@@@@%%+=+********+++##*+*+:::::::::::=+*#+#%%%%##++*********+++++==*####\n");
            printf("@@@@@@@@@@%%+=+*********++=-::::::::::::::::--:-++++**###*********++++=*%%%%%%\n");
            printf("@@@@@@@@@@%%+++********+=-:::::::::::::::::::::::-=++**************++==+%%%%%%\n");
            printf("@@@@@@@@@@%%#++*+++++*+=-:::::::::::::::::::::::::-==++******++*****+==*%%%%%%\n");
            printf("@@@@@@@@@@##++++++++++=---:::::=*+++*+=-:::::::::::-=++++++++++++*++==*%%%%%%\n");
            printf("@@@@@@%%%%@%%%%@@%%%%*+++++++==-------:---=++=--::::::::::::--==+****++==++*++=+%%%%%%\n");
            printf("@@@@@@@@@@@@%%==++======-----------=+----::::::::::::---=++++++++++++==+%%%%%%\n");
            printf("@@@@@@@@@@%%#*+====---=----------===+=---:::::-:::::------=====++++++==#%%%%%%\n");
            printf("%%%%@%%%%%%%%@%%#*+++==----====-=========+++==--------::::----------==++++==*#@@@@\n");
            printf("@@@@%%%%#****++=-----==-===================---------------------======*%%%%@@@\n");
            printf("@@@@%%**+**+==--------========================--------------------=+*%%%%%%%\n");
            printf("@%%@#*++++++=----------=======================--------------------+#%%%%@@@@@\n");
            printf("@%%#***+++==-------------=====================----------------:::-+#@@@%%%%@@@\n");
            printf("%%%%#****+*==----------------============----====--------------::::-**##%%@@@%%%%@\n");
            printf("****+**+=-------------------------------------------------::::::-+#%%%%#*%%%%@@\n");
            printf("*******+=-----------------------------------------------:::::::::+%%%%@%%#%%%%#\n");
            printf("*******+=---------------------------------------------::::::::::-*%%%%@@%%%%%@\n");
            printf("*******+---------------------------------------------:::::::::::-#%%@@%%@@@@@\n");
            printf("*#####+=-------------------------------------------:::::::::::::=#%%@@@@@@@@\n");
            printf("*####*+=--------------------------------------:::::::::::::::::::+#%%@@@@@@@\n");
            printf("####**++==-------:------:-------------------:::::::::::::::::::::+#%%@@@@@@@\n");
            fflush(stdout);
            printf("\nYaHnI oLaN vArMi?\n");
        }
        else if (strcmp(komut, "psc") == 0) {
            char *psc_args[3] = {"/pcg-startup/psc", NULL, NULL};
            if (arguman != NULL) {
                psc_args[1] = arguman;
                psc_args[2] = NULL;
            }
            pid_t psc_pid = fork();
            if (psc_pid == 0) {
                execv("/pcg-startup/psc", psc_args);
                perror("psc baslatilmadi");
                exit(EXIT_FAILURE);
            } else if (psc_pid > 0) {
                wait(NULL);
            } else {
                perror("fork basarisiz");
            }
        }
        else if (strcmp(komut, "2048") == 0) {
            pid_t two_pid = fork();
            if (two_pid == 0) {
                setenv("TERMINFO", "/usr/lib/terminfo", 1);
                setenv("TERM", "xterm", 1);
                char *args[] = {"/bin/2048", NULL};
                execv("/bin/2048", args);
            } else if (two_pid > 0) {
                wait(NULL);
                system("reset");
            } else {
                printf("fork hatasi!\n");
            }
        }
        else if (strcmp(komut, "grafik") == 0) {
            pid_t tri_pid = fork();
            if (tri_pid == 0) {
                char *args[] = {"/bin/grafik", NULL};
                execv("/bin/grafik", args);
                perror("execv hatasi");
                exit(1);
            } else if (tri_pid < 0) {
                printf("fork hatasi!\n");
            }
        }
        else if (strcmp(komut, "karabas") == 0) {
            struct dirent **namelist;
            int n = scandir("/dev/pcgconfigs", &namelist, NULL, alphasort);
            if (n < 0) {
                printf("pcgconfigs acilamadi!\n");
                continue;
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
        else if (strcmp(komut, "yardim") == 0) {
            printf("\nKomutlar:\n");
            printf("  pati     = Supriz komutu\n");
            printf("  uname    = Sistem bilgisi\n");
            printf("  cat      = Dosya okur\n");
            printf("  temizle  = Ekrani temizler\n");
            printf("  ls       = Klasorleri listeler\n");
            printf("  mamakabi = RAM bilgisi\n");
            printf("  psc      = Guvenlik kontrol sistemi\n");
            printf("  2048     = Oyun zamanii!\n");
            printf("  karabas  = Calisan processleri listeler\n");
            printf("  miyav = miyav <adres> (ping)\n");
            printf("  libreturks  = ahem...\n");
            printf("  touch = dosya olusturur\n");
            printf("  cikis    = Sistemi kapat\n\n");
        }
        else {
            printf("Komut bulunamadi, bi' yardim komutuna bak...\n");
        }
    }
}
