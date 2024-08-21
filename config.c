#include "headers.h"

// Declare global variables
char username[USERNAME_MAX];
char systemname[SYSTEMNAME_MAX];
char homedirectory[PATH_MAX];

int configure() {
    // TODO: Make error function and pass errors as args and print them in error color
    if (getlogin_r(username, 4096) != 0) {
        printf("Error extracting username\n");
        return 1;
    }

    if (gethostname(systemname, 4096) != 0) {
        printf("Error extracting system name\n");
        return 1;
    }

    if (getcwd(homedirectory, 4096) == NULL) {
        printf("Error extracting home directory\n");
        return 1;
    }

    return 0;
}