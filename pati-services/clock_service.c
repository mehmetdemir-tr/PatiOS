#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>

int main() {
    printf("Saat servisi baslatiliyor...\n");
    fflush(stdout);
    int fd = open("/dev/rtc0", O_RDONLY);
    struct rtc_time rtc_tm;

    int ret = ioctl(fd, RTC_RD_TIME, &rtc_tm);
    printf("Bismillahirahmanirrahim: %d-%d-%d %02d:%02d:%02d\n",
    rtc_tm.tm_mday, rtc_tm.tm_mon + 1, rtc_tm.tm_year + 1900,
    rtc_tm.tm_hour + 3, rtc_tm.tm_min, rtc_tm.tm_sec);
    fflush(stdout);
    close(fd);
    
}
