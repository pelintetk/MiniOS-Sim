
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include "parser.h"

#define MAX_LINE_LENGTH 512

static int parse_integer(const char *text, int *result) {
    char *end;
    errno = 0;

    long value = strtol(text, &end, 10);

    if (text == end || *end != '\0' ||
        errno == ERANGE || value < INT_MIN || value > INT_MAX) {
        return -1;
    }

    *result = (int)value;
    return 0;
}

int load_workload(const char *filename, PCB processes[], int max_processes) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        fprintf(stderr, "Error: cannot open input file\n");
        return -1;
    }

    char line[MAX_LINE_LENGTH];
    int count = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        size_t length = strlen(line);

        if (length > 0 && line[length - 1] != '\n' && !feof(file)) {
            fprintf(stderr, "Error: input line too long\n");
            fclose(file);
            return -1;
        }

        char *cursor = line;
        while (isspace((unsigned char)*cursor)) {
            cursor++;
        }

        if (*cursor == '#' || *cursor == '\0') {
            continue;
        }

        if (count >= max_processes) {
            fprintf(stderr, "Error: too many processes\n");
            fclose(file);
            return -1;
        }

        PCB *p = &processes[count];
        memset(p, 0, sizeof(*p));

        char *token = strtok(cursor, " \t\r\n");

        if (token == NULL || strlen(token) >= MAX_PID_LENGTH) {
            fprintf(stderr, "Error: invalid PID\n");
            fclose(file);
            return -1;
        }

        strcpy(p->pid, token);

        token = strtok(NULL, " \t\r\n");
        if (token == NULL || parse_integer(token, &p->arrival_time) != 0 ||
            p->arrival_time < 0) {
            fprintf(stderr, "Error: invalid arrival time\n");
            fclose(file);
            return -1;
        }

        token = strtok(NULL, " \t\r\n");
        if (token == NULL || parse_integer(token, &p->priority) != 0) {
            fprintf(stderr, "Error: invalid priority\n");
            fclose(file);
            return -1;
        }

        p->current_priority = p->priority;

        while ((token = strtok(NULL, " \t\r\n")) != NULL) {
            if (p->burst_count >= MAX_BURSTS) {
                fprintf(stderr, "Error: too many bursts\n");
                fclose(file);
                return -1;
            }

            int burst;
            if (parse_integer(token, &burst) != 0 || burst <= 0) {
                fprintf(stderr, "Error: invalid burst duration\n");
                fclose(file);
                return -1;
            }

            p->bursts[p->burst_count++] = burst;
        }

        if (p->burst_count == 0 || p->burst_count % 2 == 0) {
            fprintf(stderr, "Error: bursts must start and end with CPU\n");
            fclose(file);
            return -1;
        }

        for (int i = 0; i < count; i++) {
            if (strcmp(processes[i].pid, p->pid) == 0) {
                fprintf(stderr, "Error: duplicate PID\n");
                fclose(file);
                return -1;
            }
        }

        p->current_burst = 0;
        p->remaining_time = p->bursts[0];
        p->first_start_time = -1;
        p->completion_time = -1;
        p->ready_entry_time = -1;
        p->state = NEW;

        count++;
    }

    if (ferror(file)) {
        fprintf(stderr, "Error: failed to read input file\n");
        fclose(file);
        return -1;
    }

    fclose(file);
    return count;
}
