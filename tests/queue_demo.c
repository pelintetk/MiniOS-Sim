
#include <stdio.h>
#include "../src/sched/scheduler.h"

int main(void) {
    ReadyQueue queue;
    queue_init(&queue);

    queue_push(&queue, 0);
    queue_push(&queue, 1);
    queue_push(&queue, 2);

    printf("First: %d\n", queue_pop(&queue));
    printf("Second: %d\n", queue_pop(&queue));
    printf("Third: %d\n", queue_pop(&queue));

    return 0;
}
