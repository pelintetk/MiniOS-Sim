
#include <stdio.h>
#include <string.h>
#include "parser.h"

int load_workload(const char *filename, PCB processes[], int max_processes) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        fprintf(stderr, "Error: cannot open input file\n");
        return -1;
    }

    char line[256];
    int count = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        if (line[0] == '#' || line[0] == '\n') {
            continue;
        }

        if (count >= max_processes) {
            fprintf(stderr, "Error: too many processes\n");
            fclose(file);
            return -1;
        }

        PCB *p = &processes[count];

        int fields = sscanf(
            line,
            "%31s %d %d %d",
            p->pid,
            &p->arrival_time,
            &p->priority,
            &p->bursts[0]
        );

        if (fields != 4 || p->arrival_time < 0 ||
            p->bursts[0] <= 0) {
            fprintf(stderr, "Error: invalid process input\n");
            fclose(file);
            return -1;
        }

        p->current_priority = p->priority;
        p->burst_count = 1;
        p->current_burst = 0;
        p->remaining_time = p->bursts[0];
        p->first_start_time = -1;
        p->completion_time = -1;
        p->state = NEW;

        count++;
    }

    fclose(file);
    return count;
}
