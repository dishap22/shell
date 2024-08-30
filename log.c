#include "headers.h"

int logCommand(char *cmd) {
    // Open log file, create if doesn't already exist
    FILE *file = fopen(logDirectory, "a+");
    if (file == NULL) {
        printf(ERROR_COLOR "Error opening log file\n" RESET_COLOR);
        return -1;
    }

    char logHistory[MAX_LOG_STORAGE][PATH_MAX];
    int size = 0;
    char line[PATH_MAX];

    // get data from file and store in array
    while (fgets(line, sizeof(line), file) && size < MAX_LOG_STORAGE) {
        line[strcspn(line, "\n")] = '\0';
        strncpy(logHistory[size], line, PATH_MAX);
        size++;
    }
    fclose(file);

    // if current command same as most recent one
    if (size > 0 && strcmp(logHistory[size - 1], cmd) == 0) {
        return 0;
    }

    // write to log 
    file = fopen(logDirectory, "w");
    if (file == NULL) {
        printf(ERROR_COLOR "Error opening log file\n" RESET_COLOR);
        return -1;
    }

    // if max size reached, discard older entries
    if (size >= MAX_LOG_STORAGE) {
        for (int i = 1; i < MAX_LOG_STORAGE; i++) {
            strncpy(logHistory[i - 1], logHistory[i], PATH_MAX);
        }
        strncpy(logHistory[MAX_LOG_STORAGE - 1], cmd, PATH_MAX);
    } 
    // otherwise add directly
    else {
        strncpy(logHistory[size], cmd, PATH_MAX);
        size++;
    }

    // save commands to file
    for (int i = 0; i < size; i++) {
        fprintf(file, "%s\n", logHistory[i]);
    }

    fclose(file);
    return 0;
}

// Print all commands from file in reverse order
int printLog() {
    FILE *file = fopen(logDirectory, "r");
    if (file == NULL) {
        printf(ERROR_COLOR "Error opening log file\n" RESET_COLOR);
        return -1;
    }

    char logHistory[MAX_LOG_STORAGE][PATH_MAX];
    int size = 0;
    char line[PATH_MAX];

    while (fgets(line, sizeof(line), file) && size < MAX_LOG_STORAGE) {
        line[strcspn(line, "\n")] = '\0';
        strncpy(logHistory[size], line, PATH_MAX);
        size++;
    }
    fclose(file);

    for (int i = size-1; i >= 0; i--) {
        printf("%s\n", logHistory[i]);
    }
}

// Clear log file
int logPurge() {
    FILE *file = fopen(logDirectory, "w");
    if (file == NULL) {
        printf(ERROR_COLOR "Error opening log file\n" RESET_COLOR);
        return -1;
    }
    fclose(file);
    return 0;
}

// Execute a command from the file
int logExecute(int index) {
    FILE *file = fopen(logDirectory, "r");
    if (file == NULL) {
        printf(ERROR_COLOR "Error opening log file\n" RESET_COLOR);
        return -1;
    }

    char logHistory[MAX_LOG_STORAGE][PATH_MAX];
    int size = 0;
    char line[PATH_MAX];

    while (fgets(line, sizeof(line), file) && size < MAX_LOG_STORAGE) {
        line[strcspn(line, "\n")] = '\0';
        strncpy(logHistory[size], line, PATH_MAX);
        size++;
    }
    fclose(file);

    if (index < 1 || index > size) {
        printf(ERROR_COLOR "Invalid index\n" RESET_COLOR);
        return -1;
    }

    // indexing from the back
    char *temp = strdup(logHistory[size - index]);

    // tokenizing the string
    char *tokens[CHARACTER_MAX];
    int background_flag[CHARACTER_MAX];
    char *delims = "&;";
    char *tempInput = strdup(temp);
    char *token;
    int i = 0;
    token = strtok(tempInput, delims);
    if (strstr(temp, "log") == NULL) logCommand(temp);
    while (token != NULL)
    {
        tokens[i] = strdup(token);

        size_t tokenLen = strlen(token);
        size_t offset = token - tempInput + tokenLen;

        if (temp[offset] != '\0' && strchr(delims, temp[offset]) != NULL) {
            if (temp[offset] == '&') background_flag[i] = 1;
            else background_flag[i] = 0;
        } else {
            background_flag[i] = 0;
        } 
        i++;
        token = strtok(NULL, delims);
    }
    tokens[i] = NULL; 
    background_flag[i] = -1; 
    for (int j = 0; tokens[j] != NULL; j++) {
        execute_command(tokens[j], background_flag[j]); 
        free(tokens[j]);
    }
    free(tempInput);
}