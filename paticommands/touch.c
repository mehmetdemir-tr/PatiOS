#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argv[1] == NULL) {
        printf("Kullanim: touch <dosya_adi>\n");
        return 1;
    }
    int fd = creat(argv[1], 0644);
    if (fd != -1) {
        printf("Dosya olusturuldu: %s\n", argv[1]);
        close(fd);
        sync();
    } else {
        perror("touch hatasi");
}
}

