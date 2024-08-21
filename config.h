#ifndef CONFIG_H_
#define CONFIG_H_

#define PATH_MAX 4096
#define SYSTEMNAME_MAX 4096
#define USERNAME_MAX 4096

#define PURPLE_COLOR "\033[1;35m"  
#define BLUE_COLOR "\033[1;34m"      
#define RESET_COLOR "\033[0m" 
#define ERROR_COLOR "\033[31m"

// Define global variables
extern char username[PATH_MAX];
extern char systemname[SYSTEMNAME_MAX];
extern char homedirectory[USERNAME_MAX];

int configure();

#endif 