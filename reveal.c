#include "headers.h"

void reveal(char **args) {
    int a_flag = 0;
    int l_flag = 0;
    char path[PATH_MAX] = ".";
    
    // Decide if l, a flags present
    for (int i = 0; args[i] != NULL; i++) {
        // If the argument matches just '-' then it is a path reference not a flag so don't include it
        if (args[i][0] == '-' && strcmp(args[i], "-") != 0) {
            for (int j = 1; args[i][j] != '\0'; j++) {
                if (args[i][j] == 'a') {
                    a_flag = 1;
                } else if (args[i][j] == 'l') {
                    l_flag = 1;
                }
            }
        } else {
            strncpy(path, args[i], sizeof(path));
        }
    }

    // Path cases
    if (strcmp(path, "~") == 0) {
        strncpy(path, homedirectory, sizeof(path));
    } else if (strcmp(path, "-") == 0) {
        strncpy(path, previousdirectory, sizeof(path));
    } else if (path[0] != '/') {
        // relative path
        char temp[PATH_MAX * 2];
        snprintf(temp, sizeof(temp), "%s/%s", currentdirectory, path);
        strncpy(path, temp, sizeof(path));
    }
    // if path is absolute, '.' or '..', it will already be stored in the variable and we don't need to process it further
}