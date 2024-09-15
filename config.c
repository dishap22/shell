#include "headers.h"

// Declare global variables
char username[USERNAME_MAX];
char systemname[SYSTEMNAME_MAX];
char homedirectory[PATH_MAX];
char previousdirectory[CHARACTER_MAX];
char currentdirectory[CHARACTER_MAX];
char logDirectory[PATH_MAX * 2];
char neonateActive[PATH_MAX * 2];

BackgroundProcess bg_processes[MAX_BG_PROCESSES];
int bg_process_count = 0;

Alias aliases[MAX_ALIASES];
int alias_count = 0;

void parse_myshrc(const char *filepath) {
    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        printf(ERROR_COLOR "Error opening .myshrc file\n" RESET_COLOR);
        return;
    }

    char line[BUFFER_SIZE];
    while (fgets(line, sizeof(line), file)) {
        char *trimmed_line = strtok(line, "\n"); 
        if (trimmed_line == NULL) continue;
        if (strncmp(trimmed_line, "//", 2) == 0 || strncmp(trimmed_line, "#", 1) == 0) continue;

        if (strncmp(trimmed_line, ALIAS_KEYWORD, strlen(ALIAS_KEYWORD)) == 0) {
            if (alias_count >= MAX_ALIASES) {
                printf(ERROR_COLOR "Maximum number of aliases reached\n" RESET_COLOR);
                break;
            }

            char *alias_part = trimmed_line + strlen(ALIAS_KEYWORD) + 1; // move the pointer past the keyword
            char *name = strtok(alias_part, "="); // command name is len string till =
            char *command = strtok(NULL, "="); // command actual form is len string after =

            if (name && command) {
                name = trim_whitespace(name);
                command = trim_whitespace(command);
                strcpy(aliases[alias_count].name, name);
                strcpy(aliases[alias_count].command, command);
                alias_count++;
            }
        }
    }
    fclose(file);
}

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
    for (int i = 0; i < MAX_BG_PROCESSES; i++) bg_processes[i].pid = -1;

    strncpy(currentdirectory, homedirectory, sizeof(currentdirectory));
    strncpy(previousdirectory, "", sizeof(previousdirectory));
    snprintf(logDirectory, PATH_MAX * 2, "%s/%s", homedirectory, LOG_FILE);
    snprintf(neonateActive, PATH_MAX * 2, "%s/%s", homedirectory, NEONATE_FILE);
    FILE *file = fopen(logDirectory, "a");
    if (file == NULL) 
    {
        printf(ERROR_COLOR "Error creating log file\n" RESET_COLOR);
        return 1;
    }
    fclose(file);

    char myshrc_path[PATH_MAX * 2];
    snprintf(myshrc_path, PATH_MAX * 2, "%s/%s", homedirectory, MYSHRC_FILE);
    parse_myshrc(myshrc_path);

    return 0;
}