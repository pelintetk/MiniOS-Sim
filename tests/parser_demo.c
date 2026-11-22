
#include <stdio.h>
#include "../src/sched/parser.h"

int main(void) {
    PCB processes[10] = {0};

    int count = load_workload(
        "tests/sched-basic.txt",
        processes,
        10
    );

    if (count < 0) {
        return 1;
    }

    printf("Loaded %d processes\n", count);

    for (int i = 0; i < count; i++) {
        printf(
            "%s arrival=%d priority=%d cpu=%d\n",
            processes[i].pid,
            processes[i].arrival_time,
            processes[i].priority,
            processes[i].bursts[0]
        );
    }

    return 0;
}
