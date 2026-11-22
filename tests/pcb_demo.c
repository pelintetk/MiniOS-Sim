
#include <stdio.h>
#include <string.h>
#include "../src/sched/scheduler.h"

int main(void) {
    PCB p1 = {0}; //pcb veri tipinde p1 isimli process 

    strcpy(p1.pid, "P1"); //process kimliği

    p1.arrival_time = 0;
    p1.priority = 3;
    p1.current_priority = 3;

    p1.bursts[0] = 5;
    p1.burst_count = 1;
    p1.current_burst = 0;
    p1.remaining_time = 5;

    p1.first_start_time = -1;
    p1.completion_time = -1;

    p1.state = NEW;

    printf("PID: %s\n", p1.pid);
    printf("Arrival Time: %d\n", p1.arrival_time);
    printf("Priority: %d\n", p1.priority);
    printf("CPU Burst: %d\n", p1.bursts[0]);
    printf("Remaining Time: %d\n", p1.remaining_time);
    printf("State: %d\n", p1.state);

    return 0;
}
