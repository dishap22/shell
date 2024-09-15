#include "headers.h"

void fileInfo(struct dirent *entry, struct stat *file_stat) {
    // file permissions
    printf((S_ISDIR(file_stat->st_mode)) ? "d" : "-");
    printf((file_stat->st_mode & S_IRUSR) ? "r" : "-");
    printf((file_stat->st_mode & S_IWUSR) ? "w" : "-");
    printf((file_stat->st_mode & S_IXUSR) ? "x" : "-");
    printf((file_stat->st_mode & S_IRGRP) ? "r" : "-");
    printf((file_stat->st_mode & S_IWGRP) ? "w" : "-");
    printf((file_stat->st_mode & S_IXGRP) ? "x" : "-");
    printf((file_stat->st_mode & S_IROTH) ? "r" : "-");
    printf((file_stat->st_mode & S_IWOTH) ? "w" : "-");
    printf((file_stat->st_mode & S_IXOTH) ? "x " : "- ");

    // num of links
    printf("%4ld ", file_stat->st_nlink);

    // user and group names
    struct passwd *pw = getpwuid(file_stat->st_uid);
    struct group *gr = getgrgid(file_stat->st_gid);
    printf("%s %s ", pw->pw_name, gr->gr_name);

    // size
    printf("%8ld ", file_stat->st_size);

    // last modification date
    char date[80];
    strftime(date, sizeof(date), "%b %d %H:%M", localtime(&file_stat->st_mtime));
    printf("%s ", date);

    // color coding based on type of file
    if (S_ISDIR(file_stat->st_mode)) {
        printf(BLUE_COLOR "%s\n" RESET_COLOR, entry->d_name);
    } 
    else if (file_stat->st_mode & S_IXUSR) {
        printf(GREEN_COLOR "%s\n" RESET_COLOR, entry->d_name);
    } 
    else {
        printf("%s\n", entry->d_name);
    }
}

// Comparator for qsort
int entryComparision(const void *a, const void *b) {
    struct dirent **entry_a = (struct dirent **)a;
    struct dirent **entry_b = (struct dirent **)b;
    return strcasecmp((*entry_a)->d_name, (*entry_b)->d_name);
}

int strcasecmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        int diff = tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
        if (diff != 0) {
            return diff;
        }
        s1++;
        s2++;
    }
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

// Check if file or directory
int handle_path(char *path, const char *currentdirectory, const char *homedirectory, const char *previousdirectory) {
    if (strcmp(path, "~") == 0) {
        strncpy(path, homedirectory, PATH_MAX);
    } 
    else if (strcmp(path, "-") == 0) {
        strncpy(path, previousdirectory, PATH_MAX);
    } 
    else if (path[0] != '/') {
        // relative path
        char temp[PATH_MAX * 2];
        snprintf(temp, sizeof(temp), "%s/%s", currentdirectory, path);
        strncpy(path, temp, PATH_MAX);
    }
    // if path is absolute, '.' or '..', it will already be stored in the variable and we don't need to process it further

    struct stat path_stat;
    if (stat(path, &path_stat) == -1) {
        printf(ERROR_COLOR "Error retrieving file information\n" RESET_COLOR);
        return -1;
    }

    return S_ISREG(path_stat.st_mode) ? 1 : 0;
}

// Function to list the contents of a directory
int list_directory(const char *path, int l_flag, int a_flag) {
    DIR *dir = opendir(path);
    if (dir == NULL) 
    {
        printf(ERROR_COLOR "Error opening directory\n" RESET_COLOR);
        return -1;
    }
    
    struct dirent **entry_list = NULL;
    struct dirent *entry;
    struct stat file_stat;
    int entry_count = 0;
    blkcnt_t total_blocks = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (!a_flag && entry->d_name[0] == '.') continue; // only show hidden files if a_flag

        // reallocate memory for the list of entries
        entry_list = realloc(entry_list, sizeof(struct dirent *) * (entry_count + 1));
        if (entry_list == NULL) {
            printf(ERROR_COLOR "Memory allocation error\n" RESET_COLOR);
            closedir(dir);
            return -1;
        }

        // create block for new entry
        entry_list[entry_count] = malloc(sizeof(struct dirent));
        if (entry_list[entry_count] == NULL) {
            printf(ERROR_COLOR "Memory allocation error\n" RESET_COLOR);
            closedir(dir);
            return -1;
        }

        // copy data into it
        memcpy(entry_list[entry_count], entry, sizeof(struct dirent));
        entry_count++;

        // Calculate total blocks
        char complete_path[PATH_MAX * 2];
        snprintf(complete_path, sizeof(complete_path), "%s/%s", path, entry->d_name);
        if (stat(complete_path, &file_stat) == 0) {
            total_blocks += file_stat.st_blocks;
        }
    }

    // Print total blocks if the -l flag is set
    if (l_flag) {
        printf("total %ld\n", total_blocks / 2);  // Divide by 2 because st_blocks is in 512-byte blocks but display typically in 1 kb blocks
    }

    // Sort the directory entries
    qsort(entry_list, entry_count, sizeof(struct dirent *), entryComparision);

    for (int i = 0; i < entry_count; i++) 
    {
        char complete_path[PATH_MAX * 2];
        snprintf(complete_path, sizeof(complete_path), "%s/%s", path, entry_list[i]->d_name);

        // get file info
        if (stat(complete_path, &file_stat) == -1) 
        {
            printf(ERROR_COLOR "Error extracting file info\n" RESET_COLOR);
            continue;
        }

        // print detailed info if l otherwise just print directory names
        if (l_flag) 
        {
            fileInfo(entry_list[i], &file_stat);
        } 
        else 
        {
            if (S_ISDIR(file_stat.st_mode)) 
            {
                printf(BLUE_COLOR "%s\n" RESET_COLOR, entry_list[i]->d_name);
            } 
            else if (file_stat.st_mode & S_IXUSR) 
            {
                printf(GREEN_COLOR "%s\n" RESET_COLOR, entry_list[i]->d_name);
            } 
            else 
            {
                printf("%s\n", entry_list[i]->d_name);
            }
        }

        free(entry_list[i]);
    }

    free(entry_list);
    closedir(dir);
    return 0;
}

int reveal(char **args) {
    int a_flag = 0;
    int l_flag = 0;
    char path[PATH_MAX] = ".";

    // Decide if l, a flags present
    for (int i = 0; args[i] != NULL; i++) 
    {
        // If the argument matches just '-' then it is a path reference not a flag so don't include it
        if (args[i][0] == '-' && strcmp(args[i], "-") != 0) 
        {
            for (int j = 1; args[i][j] != '\0'; j++) 
            {
                if (args[i][j] == 'a') 
                {
                    a_flag = 1;
                } 
                else if (args[i][j] == 'l') 
                {
                    l_flag = 1;
                }
            }
        } 
        else 
        {
            strncpy(path, args[i], sizeof(path));
        }
    }

    // Handle path and determine if it's a file or directory
    int is_file = handle_path(path, currentdirectory, homedirectory, previousdirectory);

    // if it's a file just print the file's details
    if (is_file == 1) {
        struct stat path_stat;
        if (stat(path, &path_stat) == -1) 
        {
            printf(ERROR_COLOR "Error retrieving file information\n" RESET_COLOR);
            return -1;
        }

        struct dirent entry;
        strncpy(entry.d_name, path, sizeof(entry.d_name));
        fileInfo(&entry, &path_stat);
    } 
    else if (is_file == 0) 
    {
        int directoryStatus = list_directory(path, l_flag, a_flag);
        if (directoryStatus == -1) return -1;
    }
    return 0;
}
