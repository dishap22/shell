#include "headers.h"

pid_t get_newest_pid() {
    pid_t newest_pid = -1;
    DIR *proc_dir;
    struct dirent *entry;
    struct stat st;
    time_t newest_time = 0;

    proc_dir = opendir("/proc");
    if (proc_dir == NULL) {
        perror("Failed to open /proc");
        return -1;
    }

    while ((entry = readdir(proc_dir)) != NULL) {
        char path[PATH_MAX];
        if ((size_t) snprintf(path, sizeof(path), "/proc/%s", entry->d_name) >= sizeof(path)) {
            continue;
        }
        
        if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) {
            char *endptr;
            long pid = strtol(entry->d_name, &endptr, 10);
            if (*endptr == '\0' && pid > 0 && pid <= INT_MAX) {
                if (st.st_ctime > newest_time) {
                    newest_time = st.st_ctime;
                    newest_pid = (pid_t)pid;
                }
            }
        }
    }

    closedir(proc_dir);
    return newest_pid;
}

void neonate(int time_arg) {
    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);


    FILE *active_file = fopen(neonateActive, "w");
    if (active_file) {
        fprintf(active_file, "active");
        fclose(active_file);
    }
    while (access(neonateActive, F_OK) != -1) {
        pid_t newest_pid = get_newest_pid();
        if (newest_pid != -1) {
            printf("%d\n", newest_pid);
            fflush(stdout);
        }
        
        sleep(time_arg);
    }
}
