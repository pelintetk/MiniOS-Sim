
#include <stdio.h>
#include "scheduler.h"

// =======================
// READY QUEUE OPERATIONS
// =======================

void queue_init(ReadyQueue *queue) {
    queue->front = 0;
    queue->rear = 0;
    queue->size = 0;
}

int queue_push(ReadyQueue *queue, int process_index) {
    if (queue->size >= MAX_PROCESSES) {
        return -1;
    }

    queue->items[queue->rear] = process_index;
    queue->rear = (queue->rear + 1) % MAX_PROCESSES;
    queue->size++;

    return 0;
}

int queue_pop(ReadyQueue *queue) {
    if (queue->size == 0) {
        return -1;
    }

    int index = queue->items[queue->front];
    queue->front = (queue->front + 1) % MAX_PROCESSES;
    queue->size--;

    return index;
}

// =======================
// PROCESS STATE FUNCTIONS
// =======================

static const char *state_name(ProcessState state) {
    switch (state) {
        case NEW:        return "NEW";
        case READY:      return "READY";
        case RUNNING:    return "RUNNING";
        case WAITING:    return "WAITING";
        case TERMINATED: return "TERMINATED";
        default:         return "UNKNOWN";
    }
}

static void change_state(PCB *process, ProcessState new_state,
                         int clock, int trace_enabled) {
    if (trace_enabled) {
        printf(
            "t=%d %s %s -> %s\n",
            clock,
            process->pid,
            state_name(process->state),
            state_name(new_state)
        );
    }

    process->state = new_state;
}

// =======================
// FCFS POLICY
// =======================

static int pick_next_fcfs(ReadyQueue *queue) {
    return queue_pop(queue);
}

// =======================
// GANTT CHART
// =======================

static int record_segment(GanttSegment segments[], int *segment_count,
                          int max_segments, int process_index,
                          int start, int end) {
    if (*segment_count > 0) {
        GanttSegment *last = &segments[*segment_count - 1];

        if (last->process_index == process_index &&
            last->end == start) {
            last->end = end;
            return 0;
        }
    }

    if (*segment_count >= max_segments) {
        return -1;
    }

    segments[*segment_count].process_index = process_index;
    segments[*segment_count].start = start;
    segments[*segment_count].end = end;

    (*segment_count)++;
    return 0;
}

// =======================
// SCHEDULING METRICS
// =======================

static void print_metrics(const PCB processes[], int count, int total_time,
                          const GanttSegment segments[], int segment_count) {
    double total_turnaround = 0;
    double total_waiting = 0;
    double total_response = 0;

    printf("\nPID\tArrival\tCompletion\tTurnaround\tWaiting\tResponse\n");

    for (int i = 0; i < count; i++) {
        const PCB *p = &processes[i];

        int turnaround = p->completion_time - p->arrival_time;
        int response = p->first_start_time - p->arrival_time;

        printf("%s\t%d\t%d\t\t%d\t\t%d\t%d\n",
               p->pid,
               p->arrival_time,
               p->completion_time,
               turnaround,
               p->waiting_time,
               response);

        total_turnaround += turnaround;
        total_waiting += p->waiting_time;
        total_response += response;
    }

    printf("\nAverage Turnaround: %.2f\n", total_turnaround / count);
    printf("Average Waiting: %.2f\n", total_waiting / count);
    printf("Average Response: %.2f\n", total_response / count);

    int busy_ticks = 0;
    int context_switches = 0;
    int previous_process = -1;

    for (int i = 0; i < segment_count; i++) {
        const GanttSegment *s = &segments[i];

        if (s->process_index == -1) {
            continue;
        }

        busy_ticks += s->end - s->start;

        if (previous_process != -1 &&
            previous_process != s->process_index) {
            context_switches++;
        }

        previous_process = s->process_index;
    }

    if (total_time > 0) {
        double utilization = 100.0 * busy_ticks / total_time;
        double throughput = 100.0 * count / total_time;

        printf("CPU Utilization: %.2f%%\n", utilization);
        printf("Throughput (per 100 ticks): %.2f\n", throughput);
    }

    printf("Context Switches: %d\n", context_switches);
}

// =======================
// SHARED SCHEDULING ENGINE
// CURRENTLY SUPPORTS FCFS
// =======================

int run_scheduler(PCB processes[], int count,
                  const SchedulerConfig *config) {
    if (count <= 0 || count > MAX_PROCESSES ||
        config == NULL || config->policy != POLICY_FCFS) {
        return 1;
    }

    ReadyQueue queue;
    queue_init(&queue);

    int clock = 0;
    int completed = 0;
    int running = -1;

    GanttSegment segments[MAX_SEGMENTS];
    int segment_count = 0;

    while (completed < count) {

        // 1. Move arriving and I/O-completed processes to READY
        for (int i = 0; i < count; i++) {
            PCB *p = &processes[i];

            if (p->state == NEW && p->arrival_time == clock) {
                change_state(p, READY, clock, config->trace_enabled);
                p->ready_entry_time = clock;

                if (queue_push(&queue, i) != 0) {
                    return 1;
                }
            }

            if (p->state == WAITING &&
                p->io_completion_time == clock) {
                p->current_burst++;
                p->remaining_time = p->bursts[p->current_burst];

                change_state(p, READY, clock, config->trace_enabled);
                p->ready_entry_time = clock;

                if (queue_push(&queue, i) != 0) {
                    return 1;
                }
            }
        }

        // 2. Dispatch if CPU is free
        if (running == -1 && queue.size > 0) {
            running = pick_next_fcfs(&queue);

            PCB *p = &processes[running];

            if (p->first_start_time == -1) {
                p->first_start_time = clock;
            }

            p->waiting_time += clock - p->ready_entry_time;

            change_state(p, RUNNING, clock,
                         config->trace_enabled);
        }

        // 3. Record CPU usage and execute one tick
        if (record_segment(segments, &segment_count,
                           MAX_SEGMENTS, running,
                           clock, clock + 1) != 0) {
            fprintf(stderr, "Error: too many Gantt segments\n");
            return 1;
        }

        if (running != -1) {
            processes[running].remaining_time--;
        }

        clock++;

        // 4. Check completion of the current CPU burst
        if (running != -1 &&
            processes[running].remaining_time == 0) {
            PCB *p = &processes[running];

            if (p->current_burst + 1 < p->burst_count) {
                int io_duration =
                    p->bursts[p->current_burst + 1];

                p->io_completion_time = clock + io_duration;
                p->current_burst++;

                change_state(p, WAITING, clock,
                             config->trace_enabled);
            } else {
                p->completion_time = clock;

                change_state(p, TERMINATED, clock,
                             config->trace_enabled);

                completed++;
            }

            running = -1;
        }
    }

    // Print simulation results
    printf("Simulation completed at tick %d\n", clock);
    printf("Gantt: ");

    for (int i = 0; i < segment_count; i++) {
        const GanttSegment *s = &segments[i];

        if (s->process_index == -1) {
            printf("| IDLE %d-%d ", s->start, s->end);
        } else {
            printf("| %s %d-%d ",
                   processes[s->process_index].pid,
                   s->start, s->end);
        }
    }

    printf("|\n");

    print_metrics(processes, count, clock,
                  segments, segment_count);

    return 0;
}
