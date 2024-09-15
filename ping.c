#include "headers.h"

int ping(int pid, int signal_number) {
    signal_number %= 32; 
    if (kill(pid, signal_number) == -1) {
        printf("No such process found");
        return -1;
    }

    // signals from man page and their default actions
    switch (signal_number) {
        case SIGABRT:
        case SIGBUS:
        case SIGFPE:
        case SIGILL:
        case SIGQUIT:
        case SIGSEGV:
        case SIGSYS:
        case SIGXCPU:
        case SIGXFSZ:
            update_process_state(pid, "Core Dumped");
            break;
        case SIGALRM:
        case SIGHUP:
        case SIGINT:
        case SIGKILL:
        case SIGPIPE:
        case SIGPOLL:
        case SIGPROF:
        case SIGTERM:
        case SIGTRAP:
        case SIGUSR1:
        case SIGUSR2:
        case SIGVTALRM:
            update_process_state(pid, "Terminated");
            break;
        case SIGCHLD:
        case SIGURG:
        case SIGWINCH:
        // ingore signals don't change status
            break;
        case SIGCONT:
            for (int i = 0; i < bg_process_count; i++)
            {
                if (bg_processes[i].pid == pid)
                {
                    if (strcmp(bg_processes[i].state, "Stopped") == 0) {
                        update_process_state(pid, "Running");
                    }
                }
            }
            break;
        case SIGSTOP:
        case SIGTSTP:
        case SIGTTIN:
        case SIGTTOU:
            update_process_state(pid, "Stopped");
            break;
    }

    printf("Sent signal %d to process with pid %d\n", signal_number, pid);
    return 0;
}

void update_process_state(pid_t pid, const char* new_state) {
    for (int i = 0; i < bg_process_count; i++) {
        if (bg_processes[i].pid == pid) {
            strcpy(bg_processes[i].state, new_state);
            printf("Process %d (%s) is now %s\n", pid, bg_processes[i].command, new_state);
            break;
        }
    }
}

int fg(int pid) {
    if (kill(pid, 0) == -1) {
        printf("Process doesn't exist\n");
        return -1;
    }
    kill(pid, SIGCONT);
    tcsetpgrp(STDIN_FILENO, getpgid(pid));
    int status;
    while(waitpid(pid, &status, WUNTRACED) == -1 && errno == EINTR) {
    }
    tcsetpgrp(STDIN_FILENO, shell_pgid);
    foreground_pid = -1;
    return 0;
}

int bg(int pid) {
    for (int i = 0; i < bg_process_count; i++) {
        if (bg_processes[i].pid == pid) {
            if (strcmp(bg_processes[i].state, "Stopped") == 0) {
                if (kill(pid, SIGCONT) == -1) {
                    perror("Error sending continue signal");
                    return -1; 
                }
                strcpy(bg_processes[i].state, "Running");
                printf("[%d] %s &\n", bg_processes[i].job_number, bg_processes[i].command);
            }
            return 0;
        }
    }
    printf("No such process found\n");
    return -1;
}

void add_bg_process(pid_t pid, const char* command) {
    if (bg_process_count < MAX_BG_PROCESSES) {
        bg_processes[bg_process_count].pid = pid;
        strncpy(bg_processes[bg_process_count].command, command, CHARACTER_MAX - 1);
        bg_processes[bg_process_count].job_number = bg_process_count + 1;
        strcpy(bg_processes[bg_process_count].state, "Running");
        bg_process_count++;
    }
}

void cleanup_background_processes() {
    for (int i = 0; i < bg_process_count; i++) {
        kill(bg_processes[i].pid, SIGKILL);
    }
    bg_process_count = 0;
}

void remove_bg_process(int index) {
    if (index <= 0) return;
    for (int i = index; i < bg_process_count - 1; i++) {
        bg_processes[i] = bg_processes[i + 1];
    }
    bg_process_count--;
}