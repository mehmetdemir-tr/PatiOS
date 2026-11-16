#include <sys/reboot.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    printf("System Halted!");
    sync();
    reboot(RB_POWER_OFF);
}