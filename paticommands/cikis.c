#include <sys/reboot.h>
#include <unistd.h>

int main() {
    sync();
    reboot(RB_POWER_OFF);
}