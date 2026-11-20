
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Error: missing command\n");
        return 1;
    }

    if (strcmp(argv[1], "sched") == 0) {
        printf("CPU Scheduling Module\n");
        return 0;
    }

    fprintf(stderr, "Error: unknown command\n");
    return 1;
}
