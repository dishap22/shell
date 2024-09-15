#ifndef HEADERS_H_
#define HEADERS_H_

#define _XOPEN_SOURCE 700 // fixes errors with sigaction, clock_monotonic

// Standard Libraries
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/types.h> // blkcnt_t
#include <signal.h> 
#include <sys/wait.h>
#include <sys/resource.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <dirent.h>
#include <limits.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

// Custom Headers
#include "prompt.h"
#include "config.h"
#include "execute.h"
#include "hop.h"
#include "reveal.h"
#include "log.h"
#include "proclore.h"
#include "seek.h"
#include "activities.h"
#include "ping.h" 
#include "signals.h"
#include "neonate.h"
#include "iman.h"

int read_input(char *input, size_t size);
void reset_terminal_mode();
void set_raw_mode();
void handle_ctrl_c();
void handle_ctrl_d();
void init_shell();
char* check_and_replace_alias(const char* token);
char* trim_whitespace(char *str);

extern char foreground_command[CHARACTER_MAX];
extern int foreground_pid;
extern pid_t shell_pgid;
extern struct termios shell_tmodes;
extern int shell_terminal;
extern pid_t shell_pid;

#endif