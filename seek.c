#include "headers.h"

int seek(char **args) {
    int d_flag = 0, f_flag = 0, e_flag = 0;
    const char *search_name = NULL;
    const char *target = "."; // Default to current directory

    // Parsing flags and arguments
    int i = 0;
    while (args[i] != NULL) {
        if (args[i][0] == '-') {
            if (strchr(args[i], 'd')) d_flag = 1;
            if (strchr(args[i], 'f')) f_flag = 1;
            if (strchr(args[i], 'e')) e_flag = 1;
        } else {
            if (search_name == NULL) {
                search_name = args[i];
            } else {
                target = args[i];
            }
        }
        i++;
    }

    // if both only dirs and only files are set its invalid
    if (d_flag && f_flag) {
        printf(ERROR_COLOR "Invalid flags\n" RESET_COLOR);
        return -1;
    }

    if (search_name == NULL) {
        printf(ERROR_COLOR "No search value provided\n" RESET_COLOR);
        return -1;
    }

    // Performing search
    char *found_file = NULL, *found_dir = NULL;
    int found = search_directory(target, search_name, d_flag, f_flag, e_flag, &found_file, &found_dir);

    if (e_flag) 
    {
        if (found_file != NULL && found_dir == NULL) 
        {
            if (access(found_file, R_OK) == 0) // if we have read permissions
            {
                struct stat file_stat;
                if (stat(found_file, &file_stat) == 0) // if we got file status
                {
                    if (file_stat.st_mode & S_IXUSR) 
                    {
                        // Execute the file if it is executable
                        pid_t pid = fork();
                        if (pid == 0) 
                        {
                            execl(found_file, found_file, NULL);
                            exit(0);
                        } 
                        else if (pid > 0) 
                        {
                            wait(NULL);
                        } 
                        else 
                        {
                            printf(ERROR_COLOR "Failed to execute file\n" RESET_COLOR);
                        }
                    } 
                    else 
                    {
                        // Print the content if it is a regular file
                        FILE *file = fopen(found_file, "r");
                        if (file) 
                        {
                            char c;
                            while ((c = fgetc(file)) != EOF) {
                                putchar(c);
                            }
                            fclose(file);
                            printf("\n");
                        }
                    }
                } 
                else 
                {
                    printf(ERROR_COLOR "Failed to get file status\n" RESET_COLOR);
                }
            } 
            else 
            {
                printf(ERROR_COLOR "Missing permissions for task\n" RESET_COLOR);
            }
            free(found_file);
        } 
        // else if its a directory, move to that directory
        else if (found_dir != NULL && found_file == NULL) 
        {
            if (access(found_dir, X_OK) == 0) 
            {
                chdir(found_dir);
            } 
            else 
            {
                printf(ERROR_COLOR "Missing permissions for task\n" RESET_COLOR);
            }
            free(found_dir);
        }
    }

    // if nothing was found then report that
    if (!found) 
    {
        printf(ERROR_COLOR "No match found!\n" RESET_COLOR);
    }

    return 0;
}

// search directory and its sub-directories
int search_directory(const char *dir_path, const char *search_name, int d_flag, int f_flag, int e_flag, char **found_file, char **found_dir) {
    DIR *dir;
    struct dirent *entry;
    struct stat entry_stat;
    char path[PATH_MAX];
    int found_files = 0, found_dirs = 0;

    if (strcmp(dir_path, "~") == 0) dir_path = strdup(homedirectory);

    dir = opendir(dir_path);
    if (dir == NULL) {
        printf(ERROR_COLOR "Failed to open directory: %s\n" RESET_COLOR, strerror(errno));
        return 0;
    }

    while ((entry = readdir(dir)) != NULL) 
    {
        // skipping "." and ".." if they exist
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        // creating path of directory
        snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);
        if (stat(path, &entry_stat) == 0) {
            // if match found
            if (match_name(entry->d_name, search_name)) 
            {
                // check if type and flag match
                if ((d_flag && S_ISDIR(entry_stat.st_mode)) || (f_flag && S_ISREG(entry_stat.st_mode)) || (!d_flag && !f_flag)) 
                {
                    print_match(path, S_ISDIR(entry_stat.st_mode));
                    if (S_ISDIR(entry_stat.st_mode)) 
                    {
                        found_dirs++;
                        if (*found_dir == NULL) *found_dir = strdup(path);
                    } 
                    else 
                    {
                        found_files++;
                        if (*found_file == NULL) *found_file = strdup(path);
                    }
                }
            }
            // recursively search further directories
            if (S_ISDIR(entry_stat.st_mode)) 
            {
                if (search_directory(path, search_name, d_flag, f_flag, e_flag, found_file, found_dir)) 
                {
                    found_files++; // Increment to indicate that a match was found in a subdirectory
                }
            }
        } 
        else 
        {
            printf(ERROR_COLOR "Failed to get file status for %s: %s\n" RESET_COLOR, path, strerror(errno));
        }
    }
    closedir(dir);

    return found_files || found_dirs;
}

void print_match(const char *path, int is_dir) 
{
    if (is_dir) 
    {
        printf(BLUE_COLOR "%s" RESET_COLOR "\n", path);
    } 
    else 
    {
        printf(GREEN_COLOR "%s" RESET_COLOR "\n", path);
    }
}

int match_name(const char *entry_name, const char *search_name) {
    // finding latest occurrence of '.' to separate file extension
    const char *dot = strrchr(entry_name, '.');
    if (dot == NULL) 
    {
        // if none, can directly compare
        return strcmp(entry_name, search_name) == 0;
    } 
    else 
    {
        size_t name_len = strlen(search_name);
        // check if the entry starts with the search parameter
        // or if it contains the search parameter and then some file extension
        if (strncmp(entry_name, search_name, name_len) == 0 && (entry_name[name_len] == '\0' || entry_name[name_len] == '.')) 
        {
            return 1;
        }
    }
    return 0;
}