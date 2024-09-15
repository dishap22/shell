#ifndef CONFIG_H_
#define CONFIG_H_

#define PATH_MAX 4096
#define SYSTEMNAME_MAX 4096
#define USERNAME_MAX 4096
#define CHARACTER_MAX 4096
#define BUFFER_SIZE 100000

#define ALIAS_KEYWORD "alias"
#define MAX_ALIASES 4096

#define CTRL_C 3
#define CTRL_D 4
#define CTRL_Z 26

extern struct termios orig_termios;
extern struct termios new_termios;

#define PURPLE_COLOR "\033[1;35m"  
#define BLUE_COLOR "\033[1;34m"      
#define RESET_COLOR "\033[0m" 
#define ERROR_COLOR "\033[31m"
#define GREEN_COLOR "\033[1;32m"

#define BLOCK_SIZE 512

#define LOG_FILE "log.txt"
#define MAX_LOG_STORAGE 16
#define NEONATE_FILE "neonate.txt"
#define MYSHRC_FILE ".myshrc"

#define MAX_BG_PROCESSES 4096

typedef struct {
    pid_t pid;
    char command[CHARACTER_MAX];
    int job_number;
    char state[20]; 
} BackgroundProcess;

typedef struct {
    char name[CHARACTER_MAX];
    char command[CHARACTER_MAX];
} Alias;

// Define global variables
extern char username[PATH_MAX];
extern char systemname[SYSTEMNAME_MAX];
extern char homedirectory[USERNAME_MAX];
extern char previousdirectory[CHARACTER_MAX];
extern char currentdirectory[CHARACTER_MAX];
extern char logDirectory[PATH_MAX * 2];
extern char neonateActive[PATH_MAX * 2];

extern BackgroundProcess bg_processes[MAX_BG_PROCESSES];
extern int bg_process_count;
extern int foreground_pid;

extern Alias aliases[MAX_ALIASES];
extern int alias_count;

int configure();

#endif 