#ifndef ACTIVITIES_H_
#define ACTIVITIES_H_

void activities();
void print_job_status(int job_number, pid_t pid);
int compare_bg_processes(const void *a, const void *b);

#endif