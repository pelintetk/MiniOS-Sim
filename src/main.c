
#include <stdio.h>
#include <string.h>
#include "sched/parser.h"
#include "sched/scheduler.h"

int main(int argc, char *argv[]) {
    
    if (argc != 5 ||
        strcmp(argv[1], "sched") != 0 ||
        strcmp(argv[2], "-a") != 0) {

        fprintf(stderr, "Error: invalid arguments\n");
        return 1;
    }

    PCB processes[MAX_PROCESSES] = {0};

    int count = load_workload(
        argv[4],
        processes,
        MAX_PROCESSES
    );

    if (count <= 0) {
        if (count == 0) {
            fprintf(stderr, "Error: empty workload\n");
        }
        return 1;
    }

    SchedulingPolicy selected_policy;

    if (strcmp(argv[3], "fcfs") == 0) {
        selected_policy = POLICY_FCFS;
    } else if (strcmp(argv[3], "sjf") == 0) {
        selected_policy = POLICY_SJF;
    } else {
        fprintf(stderr, "Error: unsupported algorithm\n");
        return 1;
    }

    SchedulerConfig config = {
        .policy = selected_policy,
        .quantum = 4,
        .context_switch_cost = 0,
        .aging_interval = 0,
        .trace_enabled = 1
    };

    return run_scheduler(processes, count, &config);
}
