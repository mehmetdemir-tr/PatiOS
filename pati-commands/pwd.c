#include <stdio.h>
#include <unistd.h>

int main() {
    char yol[4096];
    if (getcwd(yol, sizeof(yol)) != NULL) {
        printf("%s\n", yol);
    } else {
        perror("Sahip sen nereye gittin yaa, kaybolduk!");
        return 1;
    }
    return 0;
}