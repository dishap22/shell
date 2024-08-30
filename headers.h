#ifndef HEADERS_H_
#define HEADERS_H_

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
#include <unistd.h>
#include <sys/resource.h>
#include <errno.h>

// Custom Headers
#include "prompt.h"
#include "config.h"
#include "execute.h"
#include "hop.h"
#include "reveal.h"
#include "log.h"
#include "proclore.h"
#include "seek.h"

#endif