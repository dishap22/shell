[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/Qiz9msrr)
# mini-project-1-template

# Description
- `activities.c` & `activities.h`: sorts and prints background processes
- `config.c` & `config.h`: define and configure system variables like root directory, system name, username, etc. and handle sigchld from background processes terminating
- `execute.c` & `execute.h`: further tokenizes each line into commands and their respective arguments. calls required functions accordingly
- `headers.h`: maintains list of all C standard and custom headers used in this shell
-  `hop.c` & `hop.h`: implements functionality to change directories
- `iman.c` & `iman.h`: uses sockets to retrieve man pages and prints them without headers
- `log.c` & `log.h`: Create/purge/write to/print from log file
- `main.c`: main file of the program, handles interaction and program flow
- `neonate.c` & `neonate.h`: implements neonate function to repeatedly get and print PID of most recent process on system, given time argument
- `ping.c` & `ping.h`: contains ping command to send signals to process, fg to hand control to a bg process, bg to resume a bg process and helper functions to add/remove bg processes and update the process state
- `proclore.c` & `proclore.h`: Gets details of a process by PID and prints them
- `prompt.c` & `prompt.h`: print prompt message based on directory 
- `reveal.c` & `reveal.h`: implements functionality to show contents of a directory
- `seek.c` & `seek.h`: implements funcionality to search for a specific file/directory
- `signals.c` & `signals.h`: signal handlers for sigchld, sigstp, sigint


# Assumptions
- Max size of any input is 4096 characters
- When more than one argument is present in a line, an error is given for any erroneous command, and any valid command is executed
- Previous working directory is only updated if it is different from the already stored one
- If hop - is executed before any valid hop command then `Error Changing- Directory` will be printed as no previous directory exists
- Similarly, for other commands, e.g. `reveal` if - flag is used but no previous directory exists, an error is thrown [Mentioned in README as specified in Doubts Document Spec 4 Q22]
- No limit assumed on files in a directory, realloc() being used to continuously update directory list [Mentioned in README as specified in Doubts Document Spec 4 Q21]
-  Only file/directory name is color coded [Mentioned in README as specified in Doubts Document Spec 4 Q23]
- Blocks are calculated as in bash, i.e. in terms of kB by dividing 512 blocks by 2
- Indexing in log file is in terms of 1 to 15 (i.e. log execute 1 executes most recent command)
- Log file is created in same directory that shell is invoked in upon startup if it doesn't already exist
- If a string with multiple commands is inputted, some foreground and some background, then the total time for the foreground processed to execute is saved and the new prompt generated, if time exceeds 2, contains the full command, but with time only from the foreground processes [Mentioned in README as specified in Doubts Document Spec 6 Q5]

e.g. 

```
<JohnDoe@SYS:~> sleep 5 & sleep 10; sleep 15;

46578
sleep exited normally (46578)

<JohnDoe@SYS:~ sleep 5 & sleep 10; sleep 15 : 25s>

```
- If an erroneous background command is obtained, we print `Command not found` error [Mentioned in README as specified in Doubts Document Spec 6 Q6]
- Unit for virtual memory used in proclore definition is kB [Mentioned in README as specified in Doubts Document Spec 7 Q6]
- If a file extension is given in search parameter then seek explicitly looks to match this, if no extension given then it ignores extension and gives all possible results [Mentioned in README as specified in Doubts Document Spec 8 Q1 & 6]
- Alias for .myshrc is follows the keyword 'alias'
- Comments will be on new lines in .myshrc
- log file stores alias and doesn't store any commands containing log in the alias
- In case of an erroroneous command with redirection it is sent to the changed output destination as per the redirection
- Command like `hop .. | wc` hops to parent directory
- Piping is implemented sequentially
- & only applied to command immediately preceding it in a pipe, and therefore can only apply to end of pipe (any other case would result in & | which is given as an error)
- Max processes assumed to be 4096
- Max aliases assumed to be 4096
- Upon Ctrl + D no exit status printed
- Neonate prints error if missing args

# AI Usage:
Screenshots of commands documented in [Screenshot PDF - Part A](./ChatGPTCommands_PartA.pdf)

Screenshots of commands documented in [Screenshot PDF - Part B](./ChatGPTCommands_PartB.pdf)