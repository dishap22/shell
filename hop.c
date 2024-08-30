#include "headers.h"

int hop(char **args) {
    char new_path[PATH_MAX * 2];
    char temp_cwd[PATH_MAX];

    // get cwd 
    if (getcwd(temp_cwd, sizeof(temp_cwd)) == NULL) 
    {
        printf(ERROR_COLOR "Failed to get current directory\n" RESET_COLOR);
        return -1;
    }

    // by default, change to home as no args
    if (args[0] == NULL) {
        if (chdir(homedirectory) != 0) 
        {
            printf(ERROR_COLOR "Error changing directory\n" RESET_COLOR);
            return -1;
        }
    } 
    else {
        for (int i = 0; args[i] != NULL; i++) 
        {
            if (strcmp(args[i], "~") == 0) 
            {
                if (chdir(homedirectory) != 0) 
                {
                    printf(ERROR_COLOR "Error changing directory\n" RESET_COLOR);
                    return -1;
                }
            } 
            else if (strcmp(args[i], "-") == 0) 
            {
                if (chdir(previousdirectory) != 0) 
                {
                    printf(ERROR_COLOR "Error changing directory\n" RESET_COLOR);
                    return -1;
                }
            }
            else if (strcmp(args[i], "..") == 0) 
            {
                if (chdir("..") != 0) 
                {
                    printf(ERROR_COLOR "Error changing directory\n" RESET_COLOR);
                    return -1;
                }
            } 
            else 
            {
                // absolute path
                if (args[i][0] == '/') 
                {
                    strncpy(new_path, args[i], sizeof(new_path));
                } 
                // relative path
                else 
                {
                    snprintf(new_path, sizeof(new_path), "%s/%s", currentdirectory, args[i]);
                }

                if (chdir(new_path) != 0) 
                {
                    printf(ERROR_COLOR "Error changing directory\n" RESET_COLOR);
                    return -1;
                }
            }
        }
    }

    if (getcwd(currentdirectory, sizeof(currentdirectory)) == NULL) 
    {
        printf(ERROR_COLOR "Failed to get update directory\n" RESET_COLOR);
        return -1;
    }

    // if prev directory has changed then update it
    if (strcmp(temp_cwd, currentdirectory) != 0) strncpy(previousdirectory, temp_cwd, sizeof(previousdirectory));
    printf("%s\n", currentdirectory);
    return 0;
}