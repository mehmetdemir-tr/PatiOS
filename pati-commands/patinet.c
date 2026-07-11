#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <linux/route.h>
#include <netdb.h>
#include <stdlib.h>
#include <sys/select.h>
#include <time.h>

struct __attribute__((packed)) dhcp_pkt {
    uint8_t  op, htype, hlen, hops;
    uint32_t xid;
    uint16_t secs, flags;
    uint32_t ciaddr, yiaddr, siaddr, giaddr;
    uint8_t  chaddr[16];
    char     sname[64], file[128];
    uint32_t magic;
    uint8_t  opts[312];
};

#define DHCPDISCOVER 1
#define DHCPOFFER    2
#define DHCPREQUEST  3
#define DHCPACK      5
#define OPT_SUBNET_MASK   1
#define OPT_ROUTER        3
#define OPT_DNS_SERVER    6
#define OPT_HOSTNAME     12
#define OPT_DHCP_TYPE    53
#define OPT_DHCP_SERVER  54
#define OPT_REQ_IP       50
#define OPT_END         255

void arayuzu_kaldir(const char *iface) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct ifreq ifr;

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, iface, IFNAMSIZ);
    ioctl(fd, SIOCGIFFLAGS, &ifr);
    ifr.ifr_flags &= ~IFF_UP;
    ioctl(fd, SIOCSIFFLAGS, &ifr);
    close(fd);
}

void ip_ata(const char *iface, const char *ip, const char *netmask) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, iface, IFNAMSIZ);

    struct sockaddr_in *addr = (struct sockaddr_in *)&ifr.ifr_addr;
    addr->sin_family = AF_INET;
    inet_pton(AF_INET, ip, &addr->sin_addr);
    ioctl(fd, SIOCSIFADDR, &ifr);

    inet_pton(AF_INET, netmask, &addr->sin_addr);
    ioctl(fd, SIOCSIFNETMASK, &ifr);

    ioctl(fd, SIOCGIFFLAGS, &ifr);
    ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
    ioctl(fd, SIOCSIFFLAGS, &ifr);
    close(fd);
}

void gateway_ekle(const char *gw_ip) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct rtentry route;
    memset(&route, 0, sizeof(route));

    struct sockaddr_in *gw = (struct sockaddr_in *)&route.rt_gateway;
    gw->sin_family = AF_INET;
    inet_pton(AF_INET, gw_ip, &gw->sin_addr);

    struct sockaddr_in *dst = (struct sockaddr_in *)&route.rt_dst;
    dst->sin_family = AF_INET;
    dst->sin_addr.s_addr = INADDR_ANY;

    struct sockaddr_in *mask = (struct sockaddr_in *)&route.rt_genmask;
    mask->sin_family = AF_INET;
    mask->sin_addr.s_addr = INADDR_ANY;

    route.rt_flags = RTF_UP | RTF_GATEWAY;
    route.rt_dev = "eth0";
    ioctl(fd, SIOCADDRT, &route);
    close(fd);
}

void dns_ekle(const char *dns_ip) {
    FILE *f = fopen("/etc/resolv.conf", "w");
    if (f) {
        fprintf(f,"nameserver %s\n", dns_ip);
        fclose(f);
    }
}

int dhcp_baslat(const char *iface) {
    int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (fd < 0) { perror("socket"); return 1; }

    setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, iface, strlen(iface));
    int broadcast = 1;
    setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));

    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(68);
    client_addr.sin_addr.s_addr = INADDR_ANY;
    bind(fd, (struct sockaddr *)&client_addr, sizeof(client_addr));

    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, iface, IFNAMSIZ);
    ioctl(fd, SIOCGIFHWADDR, &ifr);
    uint8_t *mac = (uint8_t *)ifr.ifr_hwaddr.sa_data;

    srand(time(NULL));
    uint32_t xid = rand();

    struct sockaddr_in dest;
    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(67);
    inet_pton(AF_INET, "255.255.255.255", &dest.sin_addr);

    struct dhcp_pkt pkt;
    memset(&pkt, 0, sizeof(pkt));
    pkt.op = 1;
    pkt.htype = 1;
    pkt.hlen = 6;
    pkt.xid = htonl(xid);
    pkt.flags = htons(0x8000);
    memcpy(pkt.chaddr, mac, 6);
    pkt.magic = htonl(0x63825363);

    int optlen = 0;
    pkt.opts[optlen++] = OPT_DHCP_TYPE;
    pkt.opts[optlen++] = 1;
    pkt.opts[optlen++] = DHCPDISCOVER;
    pkt.opts[optlen++] = OPT_HOSTNAME;
    pkt.opts[optlen++] = 6;
    memcpy(&pkt.opts[optlen], "patios", 6);
    optlen += 6;
    pkt.opts[optlen++] = 55;
    pkt.opts[optlen++] = 3;
    pkt.opts[optlen++] = OPT_SUBNET_MASK;
    pkt.opts[optlen++] = OPT_ROUTER;
    pkt.opts[optlen++] = OPT_DNS_SERVER;
    pkt.opts[optlen++] = OPT_END;

    sendto(fd, &pkt, sizeof(pkt), 0, (struct sockaddr *)&dest, sizeof(dest));
    printf("[DHCP] DISCOVER gönderildi...\n");

    struct dhcp_pkt reply;
    struct sockaddr_in from;
    socklen_t fromlen = sizeof(from);

    fd_set readfds;
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);

    if (select(fd + 1, &readfds, NULL, NULL, &tv) <= 0) {
        printf("[DHCP] Zaman aşımı, sunucu bulunamadı.\n");
        close(fd); return 1;
    }
    recvfrom(fd, &reply, sizeof(reply), 0, (struct sockaddr *)&from, &fromlen);
    if (ntohl(reply.magic) != 0x63825363) {
        printf("[DHCP] Geçersiz cevap.\n"); close(fd); return 1;
    }

    uint32_t subnet_mask = 0, gateway = 0, dns = 0;
    for (int i = 0; i < 312 && reply.opts[i] != OPT_END; ) {
        int t = reply.opts[i], l = reply.opts[i + 1];
        uint8_t *v = &reply.opts[i + 2];
        if (t == OPT_SUBNET_MASK && l == 4) subnet_mask = *(uint32_t *)v;
        else if (t == OPT_ROUTER && l >= 4) gateway = *(uint32_t *)v;
        else if (t == OPT_DNS_SERVER && l >= 4) dns = *(uint32_t *)v;
        i += 2 + l;
    }

    struct in_addr ipaddr = { reply.yiaddr };
    printf("[DHCP] TEKLİF: %s\n", inet_ntoa(ipaddr));

    memset(&pkt, 0, sizeof(pkt));
    pkt.op = 1; pkt.htype = 1; pkt.hlen = 6;
    pkt.xid = htonl(xid);
    pkt.flags = htons(0x8000);
    memcpy(pkt.chaddr, mac, 6);
    pkt.magic = htonl(0x63825363);

    optlen = 0;
    pkt.opts[optlen++] = OPT_DHCP_TYPE;
    pkt.opts[optlen++] = 1;
    pkt.opts[optlen++] = DHCPREQUEST;
    pkt.opts[optlen++] = OPT_REQ_IP;
    pkt.opts[optlen++] = 4;
    memcpy(&pkt.opts[optlen], &reply.yiaddr, 4);
    optlen += 4;
    pkt.opts[optlen++] = OPT_END;

    sendto(fd, &pkt, sizeof(pkt), 0, (struct sockaddr *)&dest, sizeof(dest));
    printf("[DHCP] REQUEST gönderildi...\n");

    tv.tv_sec = 5; tv.tv_usec = 0;
    FD_ZERO(&readfds); FD_SET(fd, &readfds);
    if (select(fd + 1, &readfds, NULL, NULL, &tv) <= 0) {
        printf("[DHCP] ACK zaman aşımı.\n"); close(fd); return 1;
    }
    recvfrom(fd, &reply, sizeof(reply), 0, (struct sockaddr *)&from, &fromlen);
    close(fd);

    char ip_str[16], mask_str[16], gw_str[16], dns_str[16];
    inet_ntop(AF_INET, &reply.yiaddr, ip_str, sizeof(ip_str));
    struct in_addr m = { subnet_mask };
    inet_ntop(AF_INET, &m, mask_str, sizeof(mask_str));
    struct in_addr g = { gateway };
    inet_ntop(AF_INET, &g, gw_str, sizeof(gw_str));
    struct in_addr d = { dns };
    inet_ntop(AF_INET, &d, dns_str, sizeof(dns_str));

    arayuzu_kaldir(iface);
    ip_ata(iface, ip_str, mask_str);
    gateway_ekle(gw_str);
    dns_ekle(dns_str);

    printf("[DHCP] BAŞARILI! IP=%s GW=%s DNS=%s\n", ip_str, gw_str, dns_str);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Kullanım:\n");
        printf("  patinet dhcp                    - DHCP ile IP al\n");
        printf("  patinet static IP/MASK GW DNS   - Statik yapılandırma\n");
        printf("  Örnek: patinet static 10.0.2.15/24 10.0.2.2 8.8.8.8\n");
        return 1;
    }

    if (strcmp(argv[1], "dhcp") == 0) {
        return dhcp_baslat("eth0");
    }
    if (strcmp(argv[1], "statik") == 0) {
        char ip[16], netmask[16];
        sscanf(argv[2], "%15[^/]/%%15s", ip, netmask);
        int prefix = atoi(netmask);
        uint32_t mask32 = htonl(~((1 << (32 - prefix)) - 1));
        struct in_addr m = { mask32 };
        strncpy(netmask, inet_ntoa(m), 15);
        arayuzu_kaldir("eth0");
        ip_ata("eth0", ip, netmask);
        gateway_ekle(argv[3]);
        dns_ekle(argv[4]);
        printf("[patinet] Statik: %s/%s GW=%s DNS=%s\n", ip, netmask, argv[3], argv[4]);
        return 0;
    }
    printf("Hatalı kullanım!!!\n");
    return 1;
}
