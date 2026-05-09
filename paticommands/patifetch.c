#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <time.h>

#define CYAN "\x1B[36m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define RESET "\x1B[0m"

void print_line(const char *label, const char *value) {
    printf("%s%s%s%s%s\n", CYAN, label, RESET, GREEN, value);
}

void get_kernel(char *buf, size_t sz) {
    FILE *f = fopen("/proc/version", "r");
    if (f) {
        fgets(buf, sz, f);
        char *p = strchr(buf, '(');
        if (p) *p = 0;
        fclose(f);
    } else {
        snprintf(buf, sz, "Bilinmiyor");
    }
}

void get_uptime(char *buf, size_t sz) {
    FILE *f = fopen("/proc/uptime", "r");
    if (f) {
        double sec;
        fscanf(f, "%lf", &sec);
        fclose(f);
        int d = (int)(sec / 86400);
        int h = (int)((sec - d * 86400) / 3600);
        int m = (int)((sec - d * 86400 - h * 3600) / 60);
        if (d > 0)
            snprintf(buf, sz, "%dg %ds %dm", d, h, m);
        else
            snprintf(buf, sz, "%ds %dm", h, m);
    } else {
        snprintf(buf, sz, "Bilinmiyor");
    }
}

void get_cpu(char *buf, size_t sz) {
    FILE *f = fopen("/proc/cpuinfo", "r");
    buf[0] = 0;
    if (f) {
        char line[256];
        while (fgets(line, sizeof(line), f)) {
            if (strncmp(line, "model name", 10) == 0) {
                char *p = strchr(line, ':');
                if (p) {
                    p++;
                    while (*p == ' ') p++;
                    char *nl = strchr(p, '\n');
                    if (nl) *nl = 0;
                    strncpy(buf, p, sz - 1);
                    break;
                }
            }
        }
        fclose(f);
    }
    if (!buf[0]) snprintf(buf, sz, "Bilinmiyor");
}

void get_ram(char *buf, size_t sz) {
    FILE *f = fopen("/proc/meminfo", "r");
    long total = 0, avail = 0;
    if (f) {
        char line[256];
        while (fgets(line, sizeof(line), f)) {
            if (sscanf(line, "MemTotal: %ld", &total) == 1) total /= 1024;
            if (sscanf(line, "MemAvailable: %ld", &avail) == 1) avail /= 1024;
        }
        fclose(f);
        snprintf(buf, sz, "%ldMB / %ldMB", avail, total);
    } else {
        snprintf(buf, sz, "Bilinmiyor");
    }
}

void get_ip(char *buf, size_t sz) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) { snprintf(buf, sz, "Yok"); return; }
    struct ifreq ifr;
    strcpy(ifr.ifr_name, "eth0");
    if (ioctl(fd, SIOCGIFADDR, &ifr) == 0) {
        struct sockaddr_in *sin = (struct sockaddr_in *)&ifr.ifr_addr;
        strncpy(buf, inet_ntoa(sin->sin_addr), sz - 1);
    } else {
        strcpy(ifr.ifr_name, "wlan0");
        if (ioctl(fd, SIOCGIFADDR, &ifr) == 0) {
            struct sockaddr_in *sin = (struct sockaddr_in *)&ifr.ifr_addr;
            strncpy(buf, inet_ntoa(sin->sin_addr), sz - 1);
        } else {
            snprintf(buf, sz, "Yok");
        }
    }
    close(fd);
}

void get_device_id(char *buf, size_t sz) {
    FILE *f = fopen("/etc/device.umci", "r");
    if (f) {
        fgets(buf, sz, f);
        char *nl = strchr(buf, '\n');
        if (nl) *nl = 0;
        fclose(f);
    } else {
        snprintf(buf, sz, "Tanimlanmamis");
    }
}

int get_service_count() {
    DIR *d = opendir("/dev/pcgconfigs");
    if (!d) return 0;
    int c = 0;
    struct dirent *e;
    while ((e = readdir(d)) != NULL) {
        if (strstr(e->d_name, ".pcg")) c++;
    }
    closedir(d);
    return c;
}

int main() {
    char kernel[128], uptime[64], cpu[128], ram[64], ip[64], devid[128];
    get_kernel(kernel, sizeof(kernel));
    get_uptime(uptime, sizeof(uptime));
    get_cpu(cpu, sizeof(cpu));
    get_ram(ram, sizeof(ram));
    get_ip(ip, sizeof(ip));
    get_device_id(devid, sizeof(devid));
    int services = get_service_count();

    printf("\n");
    printf(CYAN "    /\\_/\\     " RESET "pati@mobile\n");
    printf(CYAN "   ( o.o )    " RESET "----------\n");
    printf(CYAN "    > ^ <     " RESET);
    print_line(" OS:       ", "Pati Mobile OS (Strawberry)");
    printf("             ");
    print_line(" Kernel:   ", kernel);
    printf("             ");
    print_line(" Uptime:   ", uptime);
    printf("             ");
    print_line(" Shell:    ", "Pati-Shell");
    printf("             ");
    printf("%s             CPU:      %s%s\n", CYAN, YELLOW, cpu);
    printf("             ");
    print_line(" Memory:   ", ram);
    printf("             ");
    print_line(" IP:       ", ip);
    printf("             ");
    printf("%s             Device:   %s%s\n", CYAN, YELLOW, devid);
    printf("             ");
    char svc[64];
    snprintf(svc, sizeof(svc), "%d running", services);
    print_line(" Services: ", svc);
    printf("\n");

    return 0;
}
