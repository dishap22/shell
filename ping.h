#ifndef PING_H_
#define PING_H_

extern int exit_program;

int ping(int pid, int signal_number);
int fg(int job_number);
int bg(int job_number);
void update_process_state(pid_t pid, const char* new_state);
void cleanup_background_processes();
void add_bg_process(pid_t pid, const char* command);
void remove_bg_process(int index);

#endif