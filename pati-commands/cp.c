#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Kullanımı: cp <kaynak> <hedef> \n");
        return 1;
    }
    struct stat st;
    char hedef[512];
    const char *dst_path = argv[2];
    if (stat(argv[2], &st) == 0 && S_ISDIR(st.st_mode)) {
        snprintf(hedef, sizeof(hedef), "%s/%s", argv[2], argv[1]);
        dst_path = hedef;
    }
    int fd_src = open(argv[1], O_RDONLY);
    if (fd_src < 0) { perror("kaynak bulunamadı, waeow!"); return 1; }
    int fd_dst = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_dst < 0) { perror("hedef açılamadı, pati üzgün :("); close(fd_src); return 1; }

    char buf[4096];
    ssize_t n;
    while ((n = read(fd_src, buf, sizeof(buf))) > 0) {
        write(fd_dst, buf, n);
    }
    close(fd_src);
    close(fd_dst);
    return 0;
}
