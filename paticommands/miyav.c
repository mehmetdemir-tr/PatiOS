#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <netinet/ip.h>
#include <errno.h>
#include <signal.h>

#define PKT_SIZE 64
#define TIMEOUT_SEC 5
#define MAX_PAKET 5

int sock = -1;
int tx_count = 0;
int rx_count = 0;
unsigned int dest_addr;

unsigned short in_cksum(unsigned short *addr, int count) {
    unsigned long sum = 0;
    while (count > 1) {
        sum += *addr++;
        count -= 2;
    }
    if (count > 0)
        sum += *(unsigned char *)addr;
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return (~sum);
}

int init_socket() {
    int s = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (s < 0) {
        perror("socket");
        fprintf(stderr, "Hey kanka, galiba yonetici yetkisini unuttun. Onsuz ben calisamam ya!\n");
        return -1;
    }
    struct timeval tv_out;
    tv_out.tv_sec = TIMEOUT_SEC;
    tv_out.tv_usec = 0;
    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv_out, sizeof(tv_out)) < 0)
        perror("setsockopt SO_RCVTIMEO");
    return s;
}

void prep_packet(char *sendbuf, int seq) {
    memset(sendbuf, 0, PKT_SIZE);
    struct icmp *icmp_pkt = (struct icmp *)sendbuf;
    icmp_pkt->icmp_type = ICMP_ECHO;
    icmp_pkt->icmp_code = 0;
    icmp_pkt->icmp_id = getpid() & 0xFFFF;
    icmp_pkt->icmp_seq = seq;
    memset(sendbuf + sizeof(struct icmphdr), 0x42, PKT_SIZE - sizeof(struct icmphdr));
    icmp_pkt->icmp_cksum = 0;
    icmp_pkt->icmp_cksum = in_cksum((unsigned short *)icmp_pkt, PKT_SIZE);
}

int send_packet(int sock, char *sendbuf, struct sockaddr_in *dest) {
    int bytes = sendto(sock, sendbuf, PKT_SIZE, 0, (struct sockaddr *)dest, sizeof(*dest));
    if (bytes < 0)
        perror("sendto fonksiyonu basarisiz");
    return bytes;
}

int receive_packet(int sock, char *recvbuf, size_t bufsize, struct sockaddr_in *from) {
    socklen_t fromlen = sizeof(*from);
    return recvfrom(sock, recvbuf, bufsize, 0, (struct sockaddr *)from, &fromlen);
}

void process_reply(char *recvbuf, int bytes, struct sockaddr_in *from, int seq,
                   struct timeval *tv_start, struct timeval *tv_end) {
    struct ip *ip_hdr = (struct ip *)recvbuf;
    int hlen = ip_hdr->ip_hl << 2;
    struct icmp *icmp_reply = (struct icmp *)(recvbuf + hlen);

    if (icmp_reply->icmp_type == ICMP_ECHOREPLY &&
        icmp_reply->icmp_id == (getpid() & 0xFFFF) &&
        icmp_reply->icmp_seq == seq) {

        double rtt = (tv_end->tv_sec - tv_start->tv_sec) * 1000.0 +
                     (tv_end->tv_usec - tv_start->tv_usec) / 1000.0;

        printf("%d bayt, %s adresinden: icmp_seq=%d ttl=%d sure=%.2f ms\n",
               bytes - hlen,
               inet_ntoa(from->sin_addr),
               icmp_reply->icmp_seq,
               ip_hdr->ip_ttl,
               rtt);
        rx_count++;
    }
}

void cleanup(int sig) {
    printf("\n---- %s miyavsal istatistikler ----\n", inet_ntoa(*(struct in_addr *)&dest_addr));
    printf("%d paket gonderildi, %d paket alindi, %d%% kayip\n",
           tx_count, rx_count,
           tx_count > 0 ? (tx_count - rx_count) * 100 / tx_count : 0);
    if (sock != -1)
        close(sock);
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Kullanim: %s <hedef>\n", argv[0]);
        return 1;
    }

    struct hostent *host = gethostbyname(argv[1]);
    if (!host) {
        fprintf(stderr, "Host bulunamadi: %s\n", argv[1]);
        return 1;
    }

    struct sockaddr_in dest;
    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    memcpy(&dest.sin_addr, host->h_addr, host->h_length);
    dest_addr = dest.sin_addr.s_addr;

    sock = init_socket();
    if (sock < 0)
        return 1;

    signal(SIGINT, cleanup);
    signal(SIGQUIT, cleanup);

    printf("PING %s (%s): %d bayt\n", argv[1], inet_ntoa(dest.sin_addr), PKT_SIZE);

    char sendbuf[PKT_SIZE];
    char recvbuf[1024];
    struct sockaddr_in from;

    for (int i = 0; i < MAX_PAKET; i++) {
            struct timeval tv_start, tv_end;

            prep_packet(sendbuf, i);  // tx_count yerine i
            gettimeofday(&tv_start, NULL);

            if (send_packet(sock, sendbuf, &dest) < 0) {
                tx_count++;
                sleep(1);
                continue;
            }
            tx_count++;

            int bytes = receive_packet(sock, recvbuf, sizeof(recvbuf), &from);
            gettimeofday(&tv_end, NULL);

            if (bytes < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                    printf("Zaman asimi: icmp_seq=%d\n", i);
                else
                    perror("recvfrom");
            } else {
                process_reply(recvbuf, bytes, &from, i, &tv_start, &tv_end);  // tx_count-1 yerine i
            }

            sleep(1);
        }

    cleanup(0);
    return 0;
}