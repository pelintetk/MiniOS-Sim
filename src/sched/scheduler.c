
#include "scheduler.h"

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

#include <stdio.h>

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

static int pick_next_fcfs(ReadyQueue *queue) {
    return queue_pop(queue);
}

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

    while (completed < count) {

        // 1. Check newly arriving processes
        for (int i = 0; i < count; i++) {
            if (processes[i].state == NEW &&
                processes[i].arrival_time == clock) {

                change_state(&processes[i], READY,
                             clock, config->trace_enabled);

                processes[i].ready_entry_time = clock;

                if (queue_push(&queue, i) != 0) {
                    return 1;
                }
            }
        }

        // 2. Dispatch a process if the CPU is idle
        if (running == -1 && queue.size > 0) {
            running = pick_next_fcfs(&queue);

            if (processes[running].first_start_time == -1) {
                processes[running].first_start_time = clock;
            }

            change_state(&processes[running], RUNNING,
                         clock, config->trace_enabled);
        }

        // 3. Execute one CPU tick
        if (running != -1) {
            processes[running].remaining_time--;
        }

        clock++;

        // 4. Check CPU burst completion
        if (running != -1 &&
            processes[running].remaining_time == 0) {

            processes[running].completion_time = clock;

            change_state(&processes[running], TERMINATED,
                         clock, config->trace_enabled);

            completed++;
            running = -1;
        }
    }

    printf("Simulation completed at tick %d\n", clock);
    return 0;
}
