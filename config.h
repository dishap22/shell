#ifndef CONFIG_H_
#define CONFIG_H_

#define PATH_MAX 4096
#define SYSTEMNAME_MAX 4096
#define USERNAME_MAX 4096
#define CHARACTER_MAX 4096

#define PURPLE_COLOR "\033[1;35m"  
#define BLUE_COLOR "\033[1;34m"      
#define RESET_COLOR "\033[0m" 
#define ERROR_COLOR "\033[31m"
#define GREEN_COLOR "\033[1;32m"

#define BLOCK_SIZE 512

#define LOG_FILE "log.txt"
#define MAX_LOG_STORAGE 15

#define MAX_BG_PROCESSES 4096

typedef struct {
    pid_t pid;
    char command[CHARACTER_MAX];
} BackgroundProcess;

// Define global variables
extern char username[PATH_MAX];
extern char systemname[SYSTEMNAME_MAX];
extern char homedirectory[USERNAME_MAX];
extern char previousdirectory[CHARACTER_MAX];
extern char currentdirectory[CHARACTER_MAX];
extern char logDirectory[PATH_MAX * 2];

extern BackgroundProcess bg_processes[MAX_BG_PROCESSES];
extern int bg_process_count;

int configure();
void handle_sigchld(int sig);

#endif 