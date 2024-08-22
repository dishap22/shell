// prompt.c
#include "headers.h"

int prompt() {
    char cwd[PATH_MAX];
    char *relative_directory;

    // TODO: error message handling here too
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        printf("Error extracting current directory\n");
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

    printf(PURPLE_COLOR"<%s@%s:" BLUE_COLOR "%s" PURPLE_COLOR "> " RESET_COLOR, username, systemname, relative_directory);
    fflush(stdout);
    return 0;
}
