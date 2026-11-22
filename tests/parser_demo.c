
#include <stdio.h>
#include "../src/sched/parser.h"

int main(void) {
    PCB processes[10] = {0};

    int count = load_workload(
        "tests/sched-io.txt",
        processes,
        10
    );

    if (count < 0) {
        return 1;
    }

    printf("Loaded %d processes\n", count);

    for (int i = 0; i < count; i++) {
        printf("%s: ", processes[i].pid);

        for (int j = 0; j < processes[i].burst_count; j++) {
            if (j % 2 == 0) {
                printf("CPU=%d ", processes[i].bursts[j]);
            } else {
                printf("IO=%d ", processes[i].bursts[j]);
            }
        }

        printf("\n");
    }

    return 0;
}
