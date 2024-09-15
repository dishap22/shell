#include "headers.h"

int prompt(double time, const char *prevCommand) {
    char cwd[PATH_MAX];
    char *relative_directory;
    char commandEdited[PATH_MAX];

    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        printf(ERROR_COLOR "Error extracting current directory\n" RESET_COLOR);
        return 1;
    }
  
    if (strncmp(cwd, homedirectory, strlen(homedirectory)) == 0) {
        if (strlen(cwd) == strlen(homedirectory)) {
            relative_directory = "~";
        } else {
            relative_directory = cwd + strlen(homedirectory) + 1; // Skip over the home directory path
            static char temp[PATH_MAX];
            snprintf(temp, sizeof(temp), "~/%s", relative_directory);
            relative_directory = temp;
        }
    } else {
        relative_directory = cwd;
    }

    if (prevCommand && prevCommand[strlen(prevCommand) - 1] == ';') {
        snprintf(commandEdited, sizeof(commandEdited), "%s", prevCommand);
        commandEdited[strlen(commandEdited) - 1] = '\0';
    } else {
        snprintf(commandEdited, sizeof(commandEdited), "%s", prevCommand);
    }


    if (time > 2) {
        printf(PURPLE_COLOR "<%s@%s:" BLUE_COLOR "%s" PURPLE_COLOR " %s : %lds> " RESET_COLOR, username, systemname, relative_directory, commandEdited, (long)time);
    } else {
        printf(PURPLE_COLOR "<%s@%s:" BLUE_COLOR "%s" PURPLE_COLOR "> " RESET_COLOR, username, systemname, relative_directory);
    }
    fflush(stdout);
    return 0;
}
