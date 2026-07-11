#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <openssl/sha.h>
#include <sys/socket.h>
#include <netdb.h>

#define DB_DIR "/lib/psp/db"
#define CACHE_DIR "/lib/psp/cache"
#define SERVER "patiosmirror.mehmetd.workers.dev"
#define PATCHES_PATH "/patches.json"

char *strnstr(const char *haystack, const char *needle, size_t len) {
    size_t needle_len = strlen(needle);
    if (!needle_len) return (char *)haystack;
    for (size_t i = 0; i < len && haystack[i]; i++) {
        if (i + needle_len > len) break;
        if (memcmp(haystack + i, needle, needle_len) == 0)
            return (char *)(haystack + i);
    }
    return NULL;
}

int dosya_sha256(const char *path, unsigned char out[32]) {
    FILE *f = fopen(path, "rb");
    if (!f) return -1;
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    unsigned char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), f)) > 0)
        SHA256_Update(&ctx, buf, n);
    fclose(f);
    SHA256_Final(out, &ctx);
    return 0;
}

int guvenli_kopyala(const char *kaynak, const char *hedef) {
    if (rename(kaynak, hedef) == 0) return 0;
    int in = open(kaynak, O_RDONLY);
    if (in < 0) return -1;

    char dizin[512];
    strncpy(dizin, hedef, sizeof(dizin));
    char *son_slash = strrchr(dizin, '/');
    if (son_slash) {
        *son_slash = '\0';
        mkdir(dizin, 0755);
    }

    int out = open(hedef, O_WRONLY | O_CREAT | O_TRUNC, 0755);
    if (out < 0) { close(in); return -1; }
    char buf[4096];
    ssize_t n;
    while ((n = read(in, buf, sizeof(buf))) > 0) {
        if (write(out, buf, n) != n) { close(in); close(out); return -1; }
    }
    close(in);
    close(out);
    unlink(kaynak);
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

    char req[1024];
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
    buf[total] = 0;

    char *status_line = strstr(buf, " 200");
    if (!status_line || status_line > buf + 15) return -1;

    char *body = strnstr(buf, "\r\n\r\n", total);
    if (!body) return -1;
    body += 4;
    memmove(buf, body, strlen(body) + 1);
    return 0;
}

int json_oku_hash_ve_url(const char *json, const char *kurulu_dosya, unsigned char hash[32], char *url, size_t url_sz) {
    char search[512];
    snprintf(search, sizeof(search), "\"%s\"", kurulu_dosya);
    char *p = strstr(json, search);
    if (!p) return -1;

    p += strlen(search);
    while (*p && *p != '{') p++;
    if (!*p) return -1;
    char *block_start = p;

    int depth = 0;
    char *block_end = p;
    while (*block_end) {
        if (*block_end == '{') depth++;
        else if (*block_end == '}') { depth--; if (depth == 0) break; }
        block_end++;
    }
    if (depth != 0) return -1;

    char *sha_p = strnstr(block_start, "\"sha256\": \"", (size_t)(block_end - block_start));
    if (!sha_p) return -1;
    sha_p += 11;

    char hash_str[65];
    int i;
    for (i = 0; i < 64 && sha_p[i] && sha_p[i] != '"'; i++)
        hash_str[i] = sha_p[i];
    hash_str[i] = '\0';
    if (i != 64) return -1;

    for (i = 0; i < 32; i++)
        sscanf(hash_str + i * 2, "%2hhx", &hash[i]);

    char *url_p = strnstr(block_start, "\"url\": \"", (size_t)(block_end - block_start));
    if (!url_p) return -1;
    url_p += 8;

    for (i = 0; url_p[i] && url_p[i] != '"' && i < (int)url_sz - 1; i++)
        url[i] = url_p[i];
    url[i] = '\0';

    return 0;
}

int http_get_file(const char *host, const char *path, const char *outfile) {
    char buf[4096];
    struct addrinfo hints = {.ai_family = AF_INET, .ai_socktype = SOCK_STREAM};
    struct addrinfo *res;
    if (getaddrinfo(host, "80", &hints, &res)) return -1;
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) { freeaddrinfo(res); return -1; }
    if (connect(fd, res->ai_addr, res->ai_addrlen)) { close(fd); freeaddrinfo(res); return -1; }
    freeaddrinfo(res);

    char req[1024];
    snprintf(req, sizeof(req),
        "GET %s HTTP/1.0\r\nHost: %s\r\nUser-Agent: PSPTool/2.1\r\nAccept: */*\r\n\r\n",
        path, host);
    write(fd, req, strlen(req));

    int out = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0755);
    if (out < 0) { close(fd); return -1; }

    int n, body = 0, header_checked = 0;
    size_t header_len = 0;
    char header_buf[8192];

    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        if (!body) {
            if (header_len + n < sizeof(header_buf)) {
                memcpy(header_buf + header_len, buf, n);
                header_len += n;
            } else {
                close(fd); close(out); unlink(outfile); return -1;
            }

            if (!header_checked && header_len >= 15) {
                char *status_line = strstr(header_buf, " 200");
                if (!status_line || status_line > header_buf + 15) {
                    close(fd); close(out); unlink(outfile); return -1;
                }
                header_checked = 1;
            }

            char *hdr_end = strnstr(header_buf, "\r\n\r\n", header_len);
            if (hdr_end) {
                body = 1;
                hdr_end += 4;
                int remain = header_len - (hdr_end - header_buf);
                if (remain > 0) {
                    if (write(out, hdr_end, remain) != remain) { close(fd); close(out); unlink(outfile); return -1; }
                }
            }
        } else {
            if (write(out, buf, n) != n) { close(fd); close(out); unlink(outfile); return -1; }
        }
    }
    close(fd);
    close(out);
    if (!body) { unlink(outfile); return -1; }
    return 0;
}

int cmd_check() {
    printf("\e[93m");
    printf("[?] Sunucu kontrol ediliyor..\n");
    printf("\e[0m\n");

    char json[8192];
    if (http_get(SERVER, PATCHES_PATH, json, sizeof(json))) {
	printf("\e[1m\e[31m");
        printf("HATA: Sunucuya erişilemedi, lütfen daha sonra tekrar deneyin.\n");
	printf("\e[0m\n");
        return -1;
    }

    int uyumsuz = 0, tamam = 0, eksik = 0;
    const char *json_end = json + strlen(json);
    char *p = json;
    while ((p = strnstr(p, "\"", (size_t)(json_end - p))) != NULL) {
        p++;
        char *key_start = p;
        char *closing = strnstr(p, "\"", (size_t)(json_end - p));
        if (!closing || closing >= json_end) break;
        p = closing + 1;
        char *after = p;
        while (*after == ' ' || *after == '\t') after++;
        if (*after != ':') continue;
        if (key_start[0] == '/') key_start++;
        int klen = (int)(closing - key_start);
        if (klen <= 0 || klen > 255) continue;
        char dosya[256];
        memcpy(dosya, key_start, klen);
        dosya[klen] = 0;
        if (strcmp(dosya, "sha256") == 0 || strcmp(dosya, "url") == 0) continue;

        unsigned char beklenen_hash[32], gercek_hash[32];
        char dummy_url[256];
        char yol[512];
        const char *ad = strrchr(dosya, '/') ? strrchr(dosya, '/') + 1 : dosya;
        snprintf(yol, sizeof(yol), "/data/paticommands/%s", ad);
        if (json_oku_hash_ve_url(json, dosya, beklenen_hash, dummy_url, sizeof(dummy_url)))
            continue;
        if (dosya_sha256(yol, gercek_hash)) {
	    printf("\e[1m\e[31m");
            printf("[!] %s - dosya bulunamadı.\n", yol);
	    printf("\e[0m\n");
            eksik++;
            continue;
        }

        if (memcmp(beklenen_hash, gercek_hash, 32) == 0) {
	    printf("\e[92m");
            printf("[+] Doğrulandı! %s zararsız.\n", dosya);
	    printf("\e[0m\n");
            tamam++;
        } else {
	    printf("\e[1m\e[31m");
            printf("[!!!] %s uyumsuz patch dosyası.\n", dosya);
	    printf("\e[0m\n");
            uyumsuz++;
        }
    }
    printf("\nÖzet: %d tamam, %d uyumsuz, %d eksik\n", tamam, uyumsuz, eksik);
    return uyumsuz > 0 ? 2 : 0;
}

int cmd_guncelle() {
    printf("\e[93m");
    printf("[?] Güncellemeler kontrol ediliyor...\n");
    printf("\e[0m\n");
    char json[16384];
    if (http_get(SERVER, PATCHES_PATH, json, sizeof(json))) {
	printf("\e[91m");
        printf("HATA: Sunucuya erişilemedi.\n");
	printf("\e[0m\n");
        return -1;
    }

    mkdir("/lib/psp", 0755);
    mkdir("/lib/psp/yedek", 0755);
    mkdir("/lib/psp/cache", 0755);
    mkdir("/data", 0755);
    mkdir("/data/paticommands", 0755);

    int guncellenen = 0;
    const char *json_end = json + strlen(json);
    char *p = json;
    while ((p = strnstr(p, "\"", (size_t)(json_end - p))) != NULL) {
        p++;
        char *key_start = p;
        char *closing = strnstr(p, "\"", (size_t)(json_end - p));
        if (!closing || closing >= json_end) break;
        p = closing + 1;
        char *after = p;
        while (*after == ' ' || *after == '\t') after++;
        if (*after != ':') continue;
        if (key_start[0] == '/') key_start++;
        int klen = (int)(closing - key_start);
        if (klen <= 0 || klen > 255) continue;
        char dosya[256];
        memcpy(dosya, key_start, klen);
        dosya[klen] = 0;
        if (strcmp(dosya, "sha256") == 0 || strcmp(dosya, "url") == 0) continue;

        unsigned char beklenen[32], gercek[32];
        char url[256];
        if (json_oku_hash_ve_url(json, dosya, beklenen, url, sizeof(url)))
            continue;

        const char *dosya_adi = strrchr(dosya, '/') ? strrchr(dosya, '/') + 1 : dosya;
        char hedef[512];
        snprintf(hedef, sizeof(hedef), "/data/paticommands/%s", dosya_adi);

        if (dosya_sha256(hedef, gercek) == 0 && memcmp(beklenen, gercek, 32) == 0) {
            printf(" [+] %s -- güncel\n", dosya_adi);
            continue;
        }
	printf("\e[93m");
        printf(" [?] %s -- güncelleniyor...\n", dosya_adi);
	printf("\e[0m\n");
        char yedek[512];
        snprintf(yedek, sizeof(yedek), "/lib/psp/yedek/%s", dosya_adi);

        int has_old_file = (access(hedef, F_OK) == 0);
        if (has_old_file) {
            if (guvenli_kopyala(hedef, yedek) < 0) {
		printf("\e[91m");
                printf(" [!] %s -- yedekleme başarısız, atlanıyor.\n", dosya_adi);
		printf("\e[0m\n");
                continue;
            }
        }

        char cache[512];
        char urlpath[512];
        snprintf(urlpath, sizeof(urlpath), "/%s", url);
        snprintf(cache, sizeof(cache), "/lib/psp/cache/%s.tmp",
            strrchr(url, '/') ? strrchr(url, '/') + 1 : url);
        if (http_get_file(SERVER, urlpath, cache)) {
            printf(" [!] %s -- indirme başarısız, yedek geri alınıyor.\n", dosya_adi);
            if (has_old_file) guvenli_kopyala(yedek, hedef);
            continue;
        }

        if (guvenli_kopyala(cache, hedef) < 0) {
            printf(" [!] %s -- taşıma hatası, yedek geri alınıyor.\n", dosya_adi);
            if (has_old_file) guvenli_kopyala(yedek, hedef);
            continue;
        }
        chmod(hedef, 0755);

        if (dosya_sha256(hedef, gercek) == 0 && memcmp(beklenen, gercek, 32) == 0) {
	    printf("\e[92m");
            printf(" [+] %s -- güncellendi -> /data/paticommands/%s\n", dosya_adi, dosya_adi);
	    printf("\e[0m\n");
            if (has_old_file) unlink(yedek);
            guncellenen++;
        } else {
	    printf("\e[91m");
            printf(" [!] %s -- hash uyuşmazlığı, yedek geri alınıyor.\n", dosya_adi);
	    printf("\e[0m\n");
            if (has_old_file) guvenli_kopyala(yedek, hedef);
            else unlink(hedef);
        }
    }
    printf("\e[92m");
    printf("\n%d dosya güncellendi.\n", guncellenen);
    printf("\e[0m\n");
    return 0;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Kullanım: psp <komut>\n");
        printf("Komutlar:\n");
        printf("kontrolet = Sistemde eksik veya hatalı paketlerin olup olmadığını kontrol eder\n");
        printf("güncelle  = Eksik veya hatalı patchleri yükler.\n");
        return 1;
    }
    if (!strcmp(argv[1], "kontrolet") || !strcmp(argv[1], "gkontrolet"))
        return cmd_check();
    else if (!strcmp(argv[1], "güncelle") || !strcmp(argv[1], "guncelle"))
        return cmd_guncelle();
    else
        printf("Bilinmeyen komut: %s\n", argv[1]);
    return 1;
}