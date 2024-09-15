#include "headers.h"

// Handle SIGCHLD signals from terminated child processes
void handle_sigchld() {
    pid_t pid;
    int status;
    char command[PATH_MAX];
    char *delims = " ";
    char *token;

    while ((pid = waitpid(-1, &status, WNOHANG  | WUNTRACED | WCONTINUED)) > 0) {
        for (int i = 0; i < bg_process_count; i++) {
            if (bg_processes[i].pid == pid) {
                snprintf(command, sizeof(command), "%s", bg_processes[i].command);
                token = strtok(command, delims);

                if (WIFEXITED(status)) {
                    printf("%s exited normally (%d)\n", token, pid);
                    for (int j = i; j < bg_process_count - 1; j++) {
                        bg_processes[j] = bg_processes[j + 1];
                    }
                    bg_process_count--;
                    i--; 
                } else if (WIFSIGNALED(status)) {
                    printf("%s exited abnormally (%d)\n", token, pid);
                    for (int j = i; j < bg_process_count - 1; j++) {
                        bg_processes[j] = bg_processes[j + 1];
                    }
                    bg_process_count--;
                    i--; 
                } else if (WIFSTOPPED(status)) {
                    printf("%s stopped (%d)\n", token, pid);
                    strcpy(bg_processes[i].state, "Stopped");
                } else if (WIFCONTINUED(status)) {
                    printf("%s continued (%d)\n", token, pid);
                    strcpy(bg_processes[i].state, "Running");
                }
                break;
            }
        }
    }

    // if no children exist to wait for
    if (pid == -1 && errno != ECHILD) {
        perror("Error: waitpid");
    }
}

void handle_sigint() {
    if (access(neonateActive, F_OK) != -1) {
        return;
    }
    if (foreground_pid != -1) {
        kill(-foreground_pid, SIGINT);
        int status;
        waitpid(foreground_pid, &status, WNOHANG); 
        foreground_pid = -1;
    }

    printf("\n");
}

void sigtstp_handler() {
    if (access(neonateActive, F_OK) != -1) {
        return;
    }
    if (foreground_pid != -1) {
        kill(-foreground_pid, SIGTSTP);

        if (bg_process_count < MAX_BG_PROCESSES) {
            bg_processes[bg_process_count].pid = foreground_pid;
            strcpy(bg_processes[bg_process_count].command, foreground_command);
            bg_processes[bg_process_count].job_number = bg_process_count + 1;
            strncpy(bg_processes[bg_process_count].state, "Stopped", sizeof(bg_processes[bg_process_count].state) - 1);
            bg_processes[bg_process_count].state[sizeof(bg_processes[bg_process_count].state) - 1] = '\0';
            bg_process_count++;
            printf("\n[%d]+  Stopped                 %s\n", bg_process_count, foreground_command);
        } else {
            printf("Maximum number of background processes reached\n");
        }

        // Reset foreground PID and command
        foreground_pid = -1;
        foreground_command[0] = '\0';
    }
}

void setup_signal_handlers(void) {
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigchld;
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP; // Restart system calls and don't receive SIGCHLD for stopped children
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction(SIGCHLD)");
        exit(EXIT_FAILURE);
    }

    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    sa.sa_handler = sigtstp_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_flags = 0;
    if (sigaction(SIGTSTP, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    signal(SIGTTOU, SIG_IGN);
}
