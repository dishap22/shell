#include "headers.h"

int validCommand(char *command) {
    char *path_env = getenv("PATH");
    if (path_env == NULL) 
    {
        return 0;
    }

    char *path = strtok(path_env, ":");
    char cmd_path[PATH_MAX];

    while (path != NULL) {
        snprintf(cmd_path, sizeof(cmd_path), "%s/%s", path, command);
        if (access(cmd_path, X_OK) == 0) {
            return 1; 
        }
        path = strtok(NULL, ":");
    }

    return 0; 
}

double execute_command(char *cmd, int background_flag) {
    // Split command into command and arguments
    char *tokens[CHARACTER_MAX];
    char *delims = " \t";
    char *tempCmd = strdup(cmd);
    char *token;
    int i = 0;
    int execution_status = 0;
    double elapsed = 0;
    struct timespec start, end;

    token = strtok(tempCmd, delims);
    while (token != NULL) 
    {
        tokens[i++]= token;
        token = strtok(NULL, delims);
    }
    tokens[i] = NULL;

    // Start clock to track time of execution of command
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    // Now, execute each
    if (i > 0) {
        if (strcmp(tokens[0], "hop") == 0) {
            execution_status = hop(tokens + 1);
        } 
        else if (strcmp(tokens[0], "reveal") == 0) {
            execution_status = reveal(tokens + 1);
        }
        else if (strcmp(tokens[0], "log") == 0) {
            if (tokens[1] == NULL) {
                execution_status = printLog();
            }
            else if (strcmp(tokens[1], "purge") == 0) {
                execution_status = logPurge();
            }
            else if (strcmp(tokens[1], "execute") == 0) {
                if (tokens[2] != NULL) {
                    int index = atoi(tokens[2]);
                    execution_status = logExecute(index);
                } 
                else {
                    printf(ERROR_COLOR "Index not specified\n" RESET_COLOR);
                    execution_status = -1;
                }
            }
            else {
                printf(ERROR_COLOR "Command not recognized\n" RESET_COLOR);
                execution_status = -1;
            }
        } 
        else if (strcmp(tokens[0], "proclore") == 0) {
            int pid = (i > 1) ? atoi(tokens[1]) : -1;
            proclore(pid);
        }
        else if (strcmp(tokens[0], "seek") == 0) {
            execution_status = seek(tokens + 1);
        }
        else {
            if (validCommand(tokens[0])) {
                pid_t pid = fork();

                if (pid < 0) {
                    perror("fork failed");
                    free(tempCmd);
                    return -1;
                } else if (pid == 0) {
                    if (execvp(tokens[0], tokens) < 0) {
                        execution_status = -1;
                        printf(ERROR_COLOR "Command failed\n" RESET_COLOR);
                        exit(EXIT_FAILURE);
                    }
                } else {
                    if (background_flag) {
                        if (bg_process_count < MAX_BG_PROCESSES) {
                            bg_processes[bg_process_count].pid = pid;
                            snprintf(bg_processes[bg_process_count].command, sizeof(bg_processes[bg_process_count].command), "%s", cmd);
                            bg_process_count++;
                            printf("%d\n", pid); // Print PID for background process
                        } else {
                            printf(ERROR_COLOR "Max background process limit reached\n" RESET_COLOR);
                        }
                    } else {
                        int status;
                        waitpid(pid, &status, 0); 
                        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                            execution_status = 0;
                        } else {
                            execution_status = -1;
                        }
                    }
                }
            }
            else 
            {
                printf(ERROR_COLOR "Command not found\n" RESET_COLOR);
            }
        }
    }
    // end time
    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed = end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9;


    free(tempCmd);
    // if command executed successfully and wasn't a background process then we return time to run command
    if (execution_status != -1 && !background_flag) return elapsed;
    else return execution_status;
}