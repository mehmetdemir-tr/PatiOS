#include <stdio.h>
#include <dirent.h>

int main(int argc, char *argv[]) {
    struct dirent *entry;
    DIR *dirFile;
    if (argv[1] == NULL) {
        dirFile = opendir(".");
  } else {
       dirFile = opendir(argv[1]);
  }
    if (dirFile == NULL) {
        perror("Dizin yok veya acilamadi");
        return 1;
  }
    while ((entry = readdir(dirFile)) != NULL) {
           printf("%-20s\t", entry->d_name);
  }
    closedir(dirFile);
    printf("\n");
}
