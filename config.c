#include "headers.h"

// Declare global variables
char username[USERNAME_MAX];
char systemname[SYSTEMNAME_MAX];
char homedirectory[PATH_MAX];
char previousdirectory[CHARACTER_MAX];
char currentdirectory[CHARACTER_MAX];
char logDirectory[PATH_MAX * 2];

BackgroundProcess bg_processes[MAX_BG_PROCESSES];
int bg_process_count = 0;

// Get username, cwd, system name, initialize log file
int configure() {
    if (getlogin_r(username, USERNAME_MAX) != 0) 
    {
        printf(ERROR_COLOR "Error extracting username\n" RESET_COLOR);
        return 1;
    }

    if (gethostname(systemname, SYSTEMNAME_MAX) != 0) 
    {
        printf(ERROR_COLOR "Error extracting system name\n" RESET_COLOR);
        return 1;
    }

    if (getcwd(homedirectory, PATH_MAX) == NULL) 
    {
        printf("Error extracting home directory\n");
        return 1;
    }

    strncpy(currentdirectory, homedirectory, sizeof(currentdirectory));
    strncpy(previousdirectory, "", sizeof(previousdirectory));

    snprintf(logDirectory, PATH_MAX * 2, "%s/%s", homedirectory, LOG_FILE);
    FILE *file = fopen(logDirectory, "a");
    if (file == NULL) 
    {
        printf(ERROR_COLOR "Error creating log file\n" RESET_COLOR);
        return 1;
    }
    fclose(file);

    return 0;
}

// Handle SIGCHLD signals from terminated child processes
void handle_sigchld(int sig) {
    pid_t pid;
    int status;
    char command[PATH_MAX];

    char *delims = " ";
    char *token;
    // WNOHANG -> non-blocking
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) 
    {
        for (int i = 0; i < bg_process_count; i++) 
        {
            if (bg_processes[i].pid == pid) 
            {
                snprintf(command, sizeof(command), "%s", bg_processes[i].command);
                // get command
                token = strtok(command, delims);

                if (WIFEXITED(status)) 
                {
                    printf("%s exited normally (%d)\n", token, pid);
                } 
                else if (WIFSIGNALED(status)) 
                {
                    printf("%s exited abnormally (%d)\n", token, pid);
                }

                // remove terminated process from list
                for (int j = i; j < bg_process_count - 1; j++) 
                {
                    bg_processes[j] = bg_processes[j + 1];
                }
                bg_process_count--;
                break;
            }
        }
    }

    // if no children exist to wait for
    if (pid == -1 && errno != ECHILD) 
    {
        printf(ERROR_COLOR "Error: waitpid\n" RESET_COLOR);
    }
}