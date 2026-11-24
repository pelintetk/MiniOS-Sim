
#ifndef SCHEDULER_H
#define SCHEDULER_H

#define MAX_PROCESSES 100
#define MAX_BURSTS 21
#define MAX_PID_LENGTH 32

#define MAX_SEGMENTS 10000

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


typedef struct {
    int items[MAX_PROCESSES];
    int front;
    int rear;
    int size;
} ReadyQueue;

void queue_init(ReadyQueue *queue);
int queue_push(ReadyQueue *queue, int process_index);
int queue_pop(ReadyQueue *queue);

typedef enum {
    POLICY_FCFS,
    POLICY_SJF,
    POLICY_SRTF,
    POLICY_PRIORITY,
    POLICY_RR
} SchedulingPolicy;

typedef struct {
    SchedulingPolicy policy;
    int quantum;
    int context_switch_cost;
    int aging_interval;
    int trace_enabled;
} SchedulerConfig;

typedef struct {
    int process_index;
    int start;
    int end;
} GanttSegment;

int run_scheduler(PCB processes[], int count,
                  const SchedulerConfig *config);

#endif
