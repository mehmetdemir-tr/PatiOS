#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("kullanımı: rm <dosya>\n");
        return 1;
    }
    if (unlink(argv[1]) < 0) {
        perror("silinemedi");
        return 1;
    }
    if (argc)
    return 0;
}
