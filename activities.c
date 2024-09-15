#include "headers.h"

void print_job_status(int job_number, pid_t pid) {
    printf("[%d] %d\n", job_number, pid);
}

int compare_bg_processes(const void *a, const void *b) {
    const BackgroundProcess *proc1 = (const BackgroundProcess *)a;
    const BackgroundProcess *proc2 = (const BackgroundProcess *)b;
    return strcmp(proc1->command, proc2->command);
}

void activities() {
    if (bg_process_count == 0) {
        printf("No background processes.\n");
        return;
    }
    qsort(bg_processes, bg_process_count, sizeof(BackgroundProcess), compare_bg_processes);

    for (int i = 0; i < bg_process_count; i++) {
        printf("%d : %s - %s\n", bg_processes[i].pid, bg_processes[i].command, bg_processes[i].state);
    }
}