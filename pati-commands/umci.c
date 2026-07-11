// Uluslararası Mobil Cihaz ID Generator - Comments by mehmetdemir-tr
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <arpa/inet.h>
#define VERSIYON "Pati 2.1"

int main() {
    FILE *fpointer;
    unsigned char mac_bytes[6];
    char mac[20];
    char alfabe[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    char kelime[5];
    unsigned char rastgele[4];

    fpointer = fopen("/dev/urandom", "rb");
    if (fpointer == NULL) {
        perror("urandom acilamadi");
        return -1;
    }
    fread(rastgele, 1, 4, fpointer);
    fclose(fpointer);

    for (int i = 0; i < 4; i++) {
        kelime[i] = alfabe[rastgele[i] % 36];
    }
    kelime[4] = '\0';

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) { perror("socket"); return -1; }
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ);
    if (ioctl(fd, SIOCGIFHWADDR, &ifr) < 0) {
        perror("Mac Adresi yok?");
        close(fd);
        return -1;
    }
    close(fd);
    memcpy(mac_bytes, ifr.ifr_hwaddr.sa_data, 6);
    snprintf(mac, sizeof(mac), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_bytes[0], mac_bytes[1], mac_bytes[2],
             mac_bytes[3], mac_bytes[4], mac_bytes[5]);

char mac_kisim[12];
strncpy(mac_kisim, mac, 11);
mac_kisim[11] = '\0';


int sol = 0, sag = 10;
while (sol < sag) {
    char tmp = mac_kisim[sol];
    mac_kisim[sol] = mac_kisim[sag];
    mac_kisim[sag] = tmp;
    sol++;
    sag--;
}
char umci[50];
snprintf(umci, sizeof(umci), "UI-%s-%s-Pati2.1-Pineapple", kelime, mac_kisim);
// printf("%s\n", umci);

if (access("/etc/device.umci", F_OK) == 0) {
    FILE* fl = fopen("/etc/device.umci", "r");
    if (fl == NULL) { perror("umci okunamadi"); return -1; }
    char umcio[50];
    size_t okunan = fread(umcio, 1, 49, fl);
    umcio[okunan] = '\0';
    printf("Uluslararasi Mobil Cihaz ID: %s\n", umcio);
    fclose(fl);
} else {
    FILE* fl = fopen("/etc/device.umci", "w");
    if (fl == NULL) { perror("umci yazilamadi"); return -1; }
    fprintf(fl, "%s\n", umci);
    fclose(fl);
}


    return 0;
}
