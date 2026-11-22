
#ifndef SCHEDULER_H
#define SCHEDULER_H

#define MAX_PROCESSES 100
#define MAX_BURSTS 21
#define MAX_PID_LENGTH 32

typedef enum {
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
} ProcessState;

typedef struct {
    char pid[MAX_PID_LENGTH];

    int arrival_time;
    int priority;
    int current_priority;

    int bursts[MAX_BURSTS];
    int burst_count;
    int current_burst;

    int remaining_time;
    int io_completion_time;

    int first_start_time;
    int completion_time;
    int ready_entry_time;
    int waiting_time;

    ProcessState state;
} PCB;

#endif
