#include "headers.h"

// check if commmand starts/ends with | or has two || consecutively
int validCommandStructure(char *input) {
    int len = strlen(input);
    int found_pipe = 0;
    while (*input && isspace(*input)) input++;  
    if (*input == '|') return 0;

    for (int i = 0; i < len; i++) {
        if (input[i] == '|') {
            if (found_pipe) return 0;
            found_pipe = 1;
        } else if (!isspace(input[i])) {
            found_pipe = 0;
        }
    }

    while (len > 0 && isspace(input[len - 1])) len--;
    if (input[len - 1] == '|') return 0;
    return 1;
}

static int job_number = 1;

double execute_command(char *cmd, int background_flag) {
    if (!validCommandStructure(cmd)) {
        printf(ERROR_COLOR "Invalid command structure: commands cannot start or end with '|' or contain '||'" RESET_COLOR "\n");
        return -1;
    }

    char temp_file_path[PATH_MAX * 2];
    snprintf(temp_file_path, sizeof(temp_file_path), "%s/temp.txt", homedirectory);

    pid_t last_pid = -1;
    char *commands[CHARACTER_MAX];
    int command_count = 0;
    char *delims = " \t";
    char *tempCmd = strdup(cmd);
    char *token;
    int i;
    double elapsed = 0;
    int execution_status = -1;
    struct timespec start, end;

    // Separate commands by pipes
    token = strtok(tempCmd, "|");
    while (token != NULL) {
        commands[command_count++] = strdup(token);
        token = strtok(NULL, "|");
    }
    free(tempCmd);

    if (command_count == 0) {
        return -1;
    }

    // Creating the pipes to communicate
    int pipes[2 * (command_count - 1)];
    for (i = 0; i < (command_count - 1); i++) {
        if (pipe(pipes + i * 2) == -1) {
            perror("pipe failed");
            return -1;
        }
    }

    // start timer
    clock_gettime(CLOCK_MONOTONIC, &start);

    pid_t pids[command_count];

    // Alias in multi command string
    commands[i] = trim_whitespace(commands[i]);
    commands[i] = check_and_replace_alias(commands[i]);

    for (i = 0; i < command_count; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
            return -1;
        }

        if (pid == 0) {
            // resetting signal handlers for background process
            if (background_flag) {  
                signal(SIGINT, SIG_DFL);
                signal(SIGQUIT, SIG_DFL);
                signal(SIGTTIN, SIG_DFL);
                signal(SIGTTOU, SIG_DFL);
                signal(SIGCHLD, SIG_DFL);
                signal(SIGTSTP, SIG_DFL);
            }

            // handling input redirection
            if (i > 0) {
                if (dup2(pipes[(i - 1) * 2], STDIN_FILENO) == -1) {
                    perror("dup2 failed for input");
                    exit(EXIT_FAILURE);
                }
            }

            // handling output redirection
            if (i < command_count - 1) {
                if (dup2(pipes[i * 2 + 1], STDOUT_FILENO) == -1) {
                    perror("dup2 failed for output");
                    exit(EXIT_FAILURE);
                }
            }

            // close pipe file descriptors
            for (int j = 0; j < 2 * (command_count - 1); j++) {
                close(pipes[j]);
            }

            // check current and previous directories from temporary file & update accordingly
            FILE *temp_file = fopen(temp_file_path, "r");
            if (temp_file) {
                char temp_current[PATH_MAX];
                char temp_previous[PATH_MAX];

                if (fgets(temp_current, sizeof(temp_current), temp_file) && fgets(temp_previous, sizeof(temp_previous), temp_file)) {
                    // Remove newline characters
                    temp_current[strcspn(temp_current, "\n")] = 0;
                    temp_previous[strcspn(temp_previous, "\n")] = 0;

                    strncpy(currentdirectory, temp_current, sizeof(currentdirectory));
                    strncpy(previousdirectory, temp_previous, sizeof(previousdirectory));
                }

                fclose(temp_file);
            }
            if (chdir(currentdirectory) != 0) {
                printf(ERROR_COLOR "Error initializing directory\n" RESET_COLOR);
                return -1;
            }

            // tokenizing further
            char *cmd_tokens[CHARACTER_MAX];
            int cmd_token_count = 0;
            char *cmd_token = strtok(commands[i], delims);
            while (cmd_token != NULL) {
                cmd_tokens[cmd_token_count++] = cmd_token;
                cmd_token = strtok(NULL, delims);
            }
            cmd_tokens[cmd_token_count] = NULL;

            // handling redirection within the command
            int input_fd = -1, output_fd = -1;
            for (int j = 0; j < cmd_token_count; j++) {
                if (strcmp(cmd_tokens[j], "<") == 0) {
                    if (j + 1 < cmd_token_count) {
                        input_fd = open(cmd_tokens[j + 1], O_RDONLY);
                        if (input_fd < 0) {
                            perror("open input file failed");
                            exit(EXIT_FAILURE);
                        }
                        dup2(input_fd, STDIN_FILENO);
                        close(input_fd);
                        cmd_tokens[j] = NULL;
                        cmd_tokens[j + 1] = NULL;
                        j++;
                    }
                } else if (strcmp(cmd_tokens[j], ">") == 0) {
                    if (j + 1 < cmd_token_count) {
                        output_fd = open(cmd_tokens[j + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                        if (output_fd < 0) {
                            perror("open output file failed");
                            exit(EXIT_FAILURE);
                        }
                        dup2(output_fd, STDOUT_FILENO);
                        close(output_fd);
                        cmd_tokens[j] = NULL;
                        cmd_tokens[j + 1] = NULL;
                        j++;
                    }
                } else if (strcmp(cmd_tokens[j], ">>") == 0) {
                    if (j + 1 < cmd_token_count) {
                        output_fd = open(cmd_tokens[j + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
                        if (output_fd < 0) {
                            perror("open output file failed");
                            exit(EXIT_FAILURE);
                        }
                        dup2(output_fd, STDOUT_FILENO);
                        close(output_fd);
                        cmd_tokens[j] = NULL;
                        cmd_tokens[j + 1] = NULL;
                        j++;
                    }
                }
            }

            // removing any potential null pointers 
            int new_count = 0;
            for (int j = 0; j < cmd_token_count; j++) {
                if (cmd_tokens[j] != NULL) {
                    cmd_tokens[new_count++] = cmd_tokens[j];
                }
            }
            cmd_tokens[new_count] = NULL;

            // if tokens, execute cmd
            if (new_count > 0) {
                if (strcmp(cmd_tokens[0], "ping") == 0) {
                    if (cmd_tokens[1] == NULL || cmd_tokens[2] == NULL) {
                        printf(ERROR_COLOR "Usage: ping <pid> <signal_number>\n" RESET_COLOR);
                        return -1;
                    }
                    int pid = atoi(cmd_tokens[1]);
                    int signal_number = atoi(cmd_tokens[2]);
                    ping(pid, signal_number);
                } else if (strcmp(cmd_tokens[0], "fg") == 0) {
                    if (background_flag) {
                        printf(ERROR_COLOR "Cannot run in background\n" RESET_COLOR);
                        exit(EXIT_FAILURE);
                    }
                    int pid = (new_count > 1) ? atoi(cmd_tokens[1]) : -1;
                    fg(pid);
                    waitpid(pid, NULL, WUNTRACED);
                    while (foreground_pid != -1) {
                        pause();  // Wait for signals
                    }
                } else if (strcmp(cmd_tokens[0], "hop") == 0) {
                    if (background_flag) {
                        printf(ERROR_COLOR "Cannot run in background\n" RESET_COLOR);
                        exit(EXIT_FAILURE);
                    }
                    execution_status = hop(cmd_tokens + 1);
                } else if (strcmp(cmd_tokens[0], "activities") == 0) {
                    if (background_flag) {
                        printf(ERROR_COLOR "Cannot run in background\n" RESET_COLOR);
                        exit(EXIT_FAILURE);
                    }
                    activities();
                } else if (strcmp(cmd_tokens[0], "bg") == 0) {
                    if (background_flag) {
                        printf(ERROR_COLOR "Cannot run in background\n" RESET_COLOR);
                        exit(EXIT_FAILURE);
                    }
                    int pid = (new_count > 1) ? atoi(cmd_tokens[1]) : -1;
                    bg(pid);
                } else if (strcmp(cmd_tokens[0], "reveal") == 0) {
                    if (background_flag) {
                        printf(ERROR_COLOR "Cannot run in background\n" RESET_COLOR);
                        exit(EXIT_FAILURE);
                    }
                    execution_status = reveal(cmd_tokens + 1);
                } else if (strcmp(cmd_tokens[0], "log") == 0) {
                    if (background_flag) {
                        printf(ERROR_COLOR "Cannot run in background\n" RESET_COLOR);
                        exit(EXIT_FAILURE);
                    }
                    if (cmd_tokens[1] == NULL) {
                        execution_status = printLog();
                    } else if (strcmp(cmd_tokens[1], "purge") == 0) {
                        execution_status = logPurge();
                    } else if (strcmp(cmd_tokens[1], "execute") == 0) {
                        if (cmd_tokens[2] != NULL) {
                            int index = atoi(cmd_tokens[2]);
                            execution_status = logExecute(index);
                        } else {
                            printf(ERROR_COLOR "Index not specified\n" RESET_COLOR);
                            execution_status = -1;
                        }
                    } else {
                        printf(ERROR_COLOR "Command not recognized\n" RESET_COLOR);
                        execution_status = -1;
                    }
                } else if (strcmp(cmd_tokens[0], "proclore") == 0) {
                    if (background_flag) {
                        printf(ERROR_COLOR "Cannot run in background\n" RESET_COLOR);
                        exit(EXIT_FAILURE);
                    }
                    int pid = (new_count > 1) ? atoi(cmd_tokens[1]) : -1;
                    proclore(pid);
                } else if (strcmp(cmd_tokens[0], "seek") == 0) {
                    if (background_flag) {
                        printf(ERROR_COLOR "Cannot run in background\n" RESET_COLOR);
                        exit(EXIT_FAILURE);
                    }
                    execution_status = seek(cmd_tokens + 1);
                } else if (strcmp(cmd_tokens[0], "neonate") == 0) {
                    if (background_flag) {
                        printf(ERROR_COLOR "Cannot run in background\n" RESET_COLOR);
                        exit(EXIT_FAILURE);
                    }
                    if (new_count != 3 || strcmp(cmd_tokens[1], "-n") != 0) {
                        printf(ERROR_COLOR "Usage: neonate -n <time_interval>\n" RESET_COLOR);
                        exit(EXIT_FAILURE);
                    }
                    int interval = atoi(cmd_tokens[2]);
                    if (interval <= 0) {
                        printf(ERROR_COLOR "Time interval must be a positive integer\n" RESET_COLOR);
                        exit(EXIT_FAILURE);
                    }
                    neonate(interval);
                } else if (strcmp(cmd_tokens[0], "iMan") == 0) {
                    if (background_flag) {
                        printf(ERROR_COLOR "Cannot run in background\n" RESET_COLOR);
                        exit(EXIT_FAILURE);
                    }
                    if (cmd_tokens[1] == NULL) {
                        printf(ERROR_COLOR "iMan requires an argument\n" RESET_COLOR);
                        execution_status = -1;
                    } else {
                        iMan(cmd_tokens[1]);
                    }
                } else {
                    if (execvp(cmd_tokens[0], cmd_tokens) < 0) {
                        printf(ERROR_COLOR "command not found: %s\n" RESET_COLOR, cmd_tokens[0]);
                        exit(EXIT_FAILURE);
                    } else {
                        printf(ERROR_COLOR "command not found: %s\n" RESET_COLOR, cmd_tokens[0]);
                        exit(EXIT_FAILURE);
                    }
                }
            }
            exit(execution_status);
        }

        if (background_flag) {
            setpgid(pid, pid); 
        } else {
            setpgid(pid, last_pid); 
        }

        // Parent process
        pids[i] = pid;
        last_pid = pid;

        // close pipe ends in parent
        if (i > 0) {
            close(pipes[(i - 1) * 2]);
        }
        if (i < command_count - 1) {
            close(pipes[i * 2 + 1]);
        }

        if (!background_flag || i < command_count - 1) {
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                execution_status = WEXITSTATUS(status);
            }
        }
    }

    if (!background_flag) {
        foreground_pid = last_pid; 
    }

    // close any remaining pipe file descriptors
    for (i = 0; i < 2 * (command_count - 1); i++) {
        close(pipes[i]);
    }

    if (background_flag) {
        if (bg_process_count < MAX_BG_PROCESSES) {
            bg_processes[bg_process_count].pid = pids[command_count - 1];
            snprintf(bg_processes[bg_process_count].command, sizeof(bg_processes[bg_process_count].command), "%s", cmd);
            bg_processes[bg_process_count].job_number = job_number++;
            strcpy(bg_processes[bg_process_count].state, "Running");
            bg_process_count++;
            print_job_status(bg_processes[bg_process_count - 1].job_number, pids[command_count - 1]);
        } else {
            printf(ERROR_COLOR "Max background process limit reached\n" RESET_COLOR);
        }
    } else {
        // Wait for all child processes to finish
        for (i = 0; i < command_count; i++) {
            int status;
            waitpid(pids[i], &status, 0);
            if (WIFEXITED(status)) {
                execution_status = WEXITSTATUS(status);
            }
        }
    }
    // chdir at end if any change left
    FILE *temp_file = fopen(temp_file_path, "r");
    if (temp_file) {
        char temp_current[PATH_MAX];
        char temp_previous[PATH_MAX];

        if (fgets(temp_current, sizeof(temp_current), temp_file) && fgets(temp_previous, sizeof(temp_previous), temp_file)) {
            // Remove newline characters
            temp_current[strcspn(temp_current, "\n")] = 0;
            temp_previous[strcspn(temp_previous, "\n")] = 0;

            strncpy(currentdirectory, temp_current, sizeof(currentdirectory));
            strncpy(previousdirectory, temp_previous, sizeof(previousdirectory));
        }

        fclose(temp_file);
    }
    if (chdir(currentdirectory) != 0) {
        printf(ERROR_COLOR "Error initializing directory\n" RESET_COLOR);
        return -1;
    }

    // calculate run time of cmd
    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed = end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9;

    // Flush both stdout and stderr
    fflush(stdout);
    fflush(stderr);

    // Free allocated memory for commands
    for (i = 0; i < command_count; i++) {
        free(commands[i]);
    }
    remove(temp_file_path);
    return (execution_status != -1 && !background_flag) ? elapsed : execution_status;
}
