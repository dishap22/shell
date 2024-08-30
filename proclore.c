#include "headers.h"

int proclore(int pid) {
    char proc_path[PATH_MAX];
    char status[CHARACTER_MAX];
    char exe_path[PATH_MAX];
    char state;
    pid_t pgid = -1;

    // default to current process ID if none given
    if (pid == -1) {
        pid = getpid();
    }

    // Path to status file of process
    snprintf(proc_path, sizeof(proc_path), "/proc/%d/status", pid);
    FILE *status_file = fopen(proc_path, "r");
    if (status_file == NULL) 
    {
        printf(ERROR_COLOR "Failed to open /proc/%d/status\n" RESET_COLOR, pid);
        return -1;
    }

    // reading status
    while (fgets(status, sizeof(status), status_file) != NULL) 
    {
        if (strncmp(status, "Pid:", 4) == 0) 
        {
            printf("pid : %s", strchr(status, ':') + 2);
        }
        if (strncmp(status, "VmSize:", 7) == 0) 
        {
            printf("Virtual memory : %s", strchr(status, ':') + 2);
        }
        if (strncmp(status, "PPid:", 5) == 0) 
        {
            pgid = (pid_t)atoi(strchr(status, ':') + 2);
        }
    }
    fclose(status_file);

    // path to stat file of process
    snprintf(proc_path, sizeof(proc_path), "/proc/%d/stat", pid);
    FILE *stat_file = fopen(proc_path, "r");
    if (stat_file == NULL) 
    {
        printf(ERROR_COLOR "Failed to open /proc/%d/stat\n" RESET_COLOR, pid);
        return -1;
    }

    // gets state of process
    if (fscanf(stat_file, "%*d %*s %c", &state) != 1) 
    {
        printf(ERROR_COLOR "Failed to read process state from /proc/%d/stat\n" RESET_COLOR, pid);
        fclose(stat_file);
        return -1;
    }
    fclose(stat_file);

    // path to executable link of the process
    snprintf(proc_path, sizeof(proc_path), "/proc/%d/exe", pid);
    ssize_t len = readlink(proc_path, exe_path, sizeof(exe_path) - 1);
    if (len != -1) 
    {
        exe_path[len] = '\0';
    } 
    else 
    {
        strcpy(exe_path, "Unknown");
    }

    // check if its in list of bg processes
    int is_background = 0;
    for (int i = 0; i < bg_process_count; i++) 
    {
        if (bg_processes[i].pid == pid) 
        {
            is_background = 1;
            break;
        }
    }

    printf("Process Status : %c", state);
    if (is_background) 
    {
        printf("\n");
    }
    else 
    {
        printf("+\n");
    }

    printf("Process Group : %d\n", pgid);
    printf("Executable Path : %s\n", exe_path);
    return 0;
}