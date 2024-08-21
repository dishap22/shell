#include "headers.h"

// TODO: error printing
void hop(char **args) {
    if (args[0] == NULL) {
        if (chdir(homedirectory) != 0) {
            printf("Error changing directory\n");
        }
    } else {
        for (int i = 0; args[i] != NULL; i++) {
            char new_path[PATH_MAX * 2];

            if (strcmp(args[i], "~") == 0) {
                if (chdir(homedirectory) != 0) {
                    printf("Error changing directory\n");
                }
            } 
            else if (strcmp(args[i], "-") == 0) {
                if (chdir(previousdirectory) != 0) {
                    printf("Error changing directory\n");
                }
            } 
            else if (strcmp(args[i], ".") == 0 || strcmp(args[i], "../") == 0) {
                // Do nothing
            } 
            else if (strcmp(args[i], "..") == 0) {
                if (chdir("..") != 0) {
                    printf("Error changing directory\n");
                }
            } 
            else {
                if (args[i][0] == '/') {
                    strncpy(new_path, args[i], sizeof(new_path));
                } 
                else {
                    snprintf(new_path, sizeof(new_path), "%s/%s", currentdirectory, args[i]);
                }

                if (chdir(new_path) != 0) {
                    printf("Error changing directory");
                }
            }

            strncpy(previousdirectory, currentdirectory, sizeof(previousdirectory));
            if (getcwd(currentdirectory, sizeof(currentdirectory)) == NULL) {
                printf("Failed to get update directory");
            }
            printf("%s\n", currentdirectory);
        }
    }
}