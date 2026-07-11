#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/reboot.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/stat.h>

#define API_HOST "umcilock.pythonanywhere.com"
#define API_PATH "/lock?umci="
#define POLL_INTERVAL 60
#define UMCI_FILE "/etc/device.umci"
#define BUF_SIZE 4096

int read_umci(char *buf, size_t sz) {
    FILE *f = fopen(UMCI_FILE, "r");
    if (!f) return -1;
    if (!fgets(buf, sz, f)) { fclose(f); return -1; }
    fclose(f);
    size_t len = strlen(buf);
    while (len > 0 && (buf[len-1] == '\n' || buf[len-1] == '\r'))
        buf[--len] = '\0';
    return 0;
}

int http_get(const char *host, const char *path, char *buf, size_t bufsz) {
    struct addrinfo hints = {.ai_family = AF_INET, .ai_socktype = SOCK_STREAM};
    struct addrinfo *res;
    if (getaddrinfo(host, "80", &hints, &res) != 0) return -1;
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) { freeaddrinfo(res); return -1; }
    if (connect(fd, res->ai_addr, res->ai_addrlen)) { close(fd); freeaddrinfo(res); return -1; }
    freeaddrinfo(res);

    char req[2048];
    snprintf(req, sizeof(req),
        "GET %s HTTP/1.0\r\nHost: %s\r\nUser-Agent: PatiOS/2.1\r\nAccept: */*\r\n\r\n",
        path, host);
    write(fd, req, strlen(req));

    size_t total = 0;
    int n;
    while (total < bufsz - 1 && (n = read(fd, buf + total, bufsz - 1 - total)) > 0)
        total += n;
    close(fd);
    if (total <= 0) return -1;
    buf[total] = '\0';

    char *status_line = strstr(buf, " 200");
    if (!status_line || status_line > buf + 15) return -1;

    char *body = strstr(buf, "\r\n\r\n");
    if (!body) return -1;
    body += 4;
    memmove(buf, body, strlen(body) + 1);
    return 0;
}

int main() {
    char umci[64];
    if (read_umci(umci, sizeof(umci)) != 0) {
        fprintf(stderr, "umcilock: UMCI okunamadı\n");
        return 1;
    }

    char path[2048];
    snprintf(path, sizeof(path), "%s%s", API_PATH, umci);
    char buf[BUF_SIZE];


    while (1) {
        if (http_get(API_HOST, path, buf, sizeof(buf)) == 0) {
            if (strstr(buf, "shutdown") != NULL || strstr(buf, "kapat") != NULL) {
                printf("\e[H\e[J");
                printf("Cihazınız bloke edilmiştir. Lütfen mehmetd@vuhuv.com üzerinden cihaz blokesinin açılmasını talep ediniz.\n");
                printf("UMCI Numaranız: %s\n", umci);
                sync();
                sleep(1);
                reboot(RB_POWER_OFF);
                break;
            }
        }
        sleep(POLL_INTERVAL);
    }
    return 0;
}