#include "headers.h"

// TODO: error printing
void hop(char **args) {
    char new_path[PATH_MAX * 2];
    char temp_cwd[PATH_MAX];

    if (getcwd(temp_cwd, sizeof(temp_cwd)) == NULL) {
        printf("Failed to get current directory");
        return;
    }

    if (args[0] == NULL) {
        if (chdir(homedirectory) != 0) {
            printf("Error changing directory\n");
        }
    } else {
        for (int i = 0; args[i] != NULL; i++) {
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
        }
    }

    if (getcwd(currentdirectory, sizeof(currentdirectory)) == NULL) {
        printf("Failed to get update directory");
        return;
    }

    if (strcmp(temp_cwd, currentdirectory) != 0) strncpy(previousdirectory, temp_cwd, sizeof(previousdirectory));
    printf("%s\n", currentdirectory);
}