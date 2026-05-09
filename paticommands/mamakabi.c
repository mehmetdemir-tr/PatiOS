#include <stdio.h>

int main(int argv, char *argc[]) {
    char girdi1[256];
    FILE *ofile = fopen("/proc/meminfo","r");
    if (ofile == NULL) {
       printf("MAMAM YOK CALDİLAR D:\n");
       return 1;
}
    while (fgets(girdi1, 256, ofile) != NULL) {
        printf("%s", girdi1);
}
    fclose(ofile);
    printf("\n");

}
