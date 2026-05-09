#include <stdio.h>

int main(int argc, char *argv[]) {
  char girdi1[256];
  if (argv[1] == NULL) {
     printf("Kullanim: cat <dosya>\n");
     return 1;
}
  FILE *ofile = fopen(argv[1], "r");
  if (ofile == NULL) {
     printf("Acmaya calistigin dosya bombos, kalbin gibi :)\n");
     return 1;
}
  while (fgets(girdi1,256, ofile) != NULL) {
      printf("%s", girdi1);
}
fclose(ofile);
printf("\n");
}
