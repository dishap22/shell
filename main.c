#include "headers.h"

int foreground_pid = -1;
char foreground_command[CHARACTER_MAX] = "";
struct termios orig_termios;
struct termios new_termios;

pid_t shell_pgid;
struct termios shell_tmodes;
int shell_terminal;
pid_t shell_pid;

char* trim_whitespace(char *str) {
    char *end;
    while(isspace((unsigned char)*str)) str++; //trim leading spaces
    if(*str == 0) return str; // if entire string is only spaces
    end = str + strlen(str) - 1; // trailing spaces
    while(end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0'; // end = end of string so end[1] is char after -> null terminator
    return str;
}

char* check_and_replace_alias(const char* token) {
    char* result = strdup(token);  
    if (!result) {
        perror("Memory allocation failed");
        return NULL;
    }
    trim_whitespace(result);
    for (int i = 0; i < alias_count; i++) {
        char* trimmed_name = strdup(aliases[i].name);
        char* trimmed_command = strdup(aliases[i].command);
        
        if (!trimmed_name || !trimmed_command) {
            perror("Memory allocation failed");
            free(result);
            free(trimmed_name);
            free(trimmed_command);
            return NULL;
        }

        trimmed_name = trim_whitespace(trimmed_name);
        trimmed_command = trim_whitespace(trimmed_command);

        if (strcmp(result, trimmed_name) == 0) {
            free(result);
            free(trimmed_name);
            result = trimmed_command;  
            return result;
        }

        free(trimmed_name);
        free(trimmed_command);
    }

    return result; 
}

void init_shell() {
    shell_terminal = STDIN_FILENO;
    shell_pid = getpid();
    shell_pgid = getpgrp();
    if (setpgid(shell_pid, shell_pgid) < 0) {
        perror("Couldn't put the shell in its own process group");
        exit(1);
    }
    tcsetpgrp(shell_terminal, shell_pgid);
    tcgetattr(shell_terminal, &shell_tmodes);
}

void reset_terminal_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void set_raw_mode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(reset_terminal_mode);
    memcpy(&new_termios, &orig_termios, sizeof(new_termios));
    new_termios.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
    new_termios.c_cc[VMIN] = 1;
    new_termios.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_termios);
}

void handle_ctrl_c() {
    if (access(neonateActive, F_OK) != -1) {
        return;
    }
    if (foreground_pid != -1) {
        kill(foreground_pid, SIGINT); 
        foreground_pid = -1; 
    }
}

void handle_ctrl_d() {
    printf("Exiting\n");
    if (access(neonateActive, F_OK) != -1) remove(neonateActive);
    cleanup_background_processes();
    reset_terminal_mode();
    exit(0);
}

int read_input(char *input, size_t size) {
    size_t len = 0;
    char c;
    while (len < size - 1) {
        int n = read(STDIN_FILENO, &c, 1);
        if (n == -1) {
            if (errno == EINTR) 
            {
                continue; 
            }
            perror("read");
            return -1;
        } else {
            if (c == '\n')  break;
            if (c == 127 || c == 8) { 
                // Backspace (127) or Ctrl-H (8)
                if (len > 0) {
                    len--;
                    write(STDOUT_FILENO, "\b \b", 3); 
                }
            } else if ((c == 'x' || c == 'X') && access(neonateActive, F_OK) != -1) {   
                remove(neonateActive);
                return 0;
            } else if (c == CTRL_C) { // Ctrl-C
                handle_ctrl_c();
                return -1;
            } else if (c == CTRL_D) { // Ctrl-D
                handle_ctrl_d();
                return -1;
            } else { // Printable ASCII characters
                input[len++] = c;
                write(STDOUT_FILENO, &c, 1);
            }
        }
    }
    input[len] = '\0';
    return 0;
}

int main()
{
    // Initial setup
    init_shell();
    setup_signal_handlers();

    int config_status = 1; // set to not done at start
    do {
        config_status = configure();
    } while (config_status != 0);

    char *prevCommand = NULL;
    double time = 0;

    while (1)
    {
        set_raw_mode();

        if (access(neonateActive, F_OK) == -1)
        {
            config_status = prompt(time, prevCommand);
            if (config_status != 0) continue; // Re-try prompt without taking user input if it fails
        }
        time = 0;
        double execution_status = -1; // set as false by default so if there's a valid command in a multi-command, we can set as true and store the whole string

        
        char input[CHARACTER_MAX];
        int inputReturnValue = read_input(input, sizeof(input));
        if (inputReturnValue != 0 && access(neonateActive, F_OK == -1)) {
            continue; // Handle Ctrl-C or Ctrl-Z interruption
        } else if (access(neonateActive, F_OK) != -1 && (strcmp(input, "X") == 0 || strcmp(input, "x") == 0))
        {
            remove(neonateActive);
            continue;
        }

        // make sure no commands of the form & | exist
        int invalid_command = 0;
        for (int i = 0; input[i] != '\0'; i++) {
            if (input[i] == '&') {
                int j = i + 1;
                while (input[j] == ' ' || input[j] == '\t') {
                    j++;
                }
                if (input[j] == '|') {
                    invalid_command = 1;
                    break;
                }
            }
        }

        if (invalid_command) {
            printf(ERROR_COLOR "Invalid command: '&' cannot be followed by '|'" RESET_COLOR "\n");
            continue;
        }

        // Tokenising
        input[strcspn(input, "\n")] = '\0'; // null-terminate input string if newline character is present
        if (strcmp(input, "exit") == 0) 
        {
            reset_terminal_mode();
            if (access(neonateActive, F_OK) != -1) remove(neonateActive);
            printf("\n");
            return 0;
        }

        char *tokens[CHARACTER_MAX];
        int background_flag[CHARACTER_MAX];
        char *delims = "&;";
        char *tempInput = strdup(input);
        char *token;
        int i = 0;

        token = strtok(tempInput, delims);
        while (token != NULL)
        {
            tokens[i] = check_and_replace_alias(token);

            // check if background token or not
            size_t tokenLen = strlen(token);
            size_t offset = token - tempInput + tokenLen;

            if (input[offset] != '\0' && strchr(delims, input[offset]) != NULL)
            {
                if (input[offset] == '&') background_flag[i] = 1;
                else background_flag[i] = 0;
            } 
            else 
            {
                background_flag[i] = 0;
            } 
            i++;
            token = strtok(NULL, delims); // get next token
        }

        tokens[i] = NULL; 
        background_flag[i] = -1; 
        int log = 1;
        printf("\n");
        //  Finished tokenizing, now executing 
        for (int j = 0; tokens[j] != NULL; j++) 
        {
            tokens[j] = check_and_replace_alias(tokens[j]);
            strcpy(foreground_command, tokens[j]);
            if (strstr(tokens[j], "log") != NULL) log = 0; // if log in command don't log it
            double commandDuration = execute_command(tokens[j], background_flag[j]);
            if (commandDuration > 0) 
            {
                time += commandDuration;
            } // execute command only returns time for foreground processes so in main we just need to add time if it's above 0

            free(tokens[j]);
        }
        if (log) logCommand(input); // if command doesn't have log in it then log it
        if (execution_status != 0) 
        {
            free(prevCommand);
            prevCommand = strdup(input);
        }
        free(tempInput);
    }
}
