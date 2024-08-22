#include "headers.h"

void execute_command(char *cmd, int background_flag) {
    // Split command into command and arguments
    char *tokens[CHARACTER_MAX];
    char *delims = " \t";
    char *tempCmd = strdup(cmd);
    char *token;
    int i = 0;
    token = strtok(tempCmd, delims);
    while (token != NULL) 
    {
        tokens[i++]= token;
        token = strtok(NULL, delims);
    }
    tokens[i] = NULL;
    

    // Now, execute each
    if (i > 0) {
        if (strcmp(tokens[0], "hop") == 0) {
            hop(tokens + 1);
        } 
        else if (strcmp(tokens[0], "reveal") == 0) {
            reveal(tokens + 1);
        }
        else {
            printf("Command not recognized\n");
        }
    }
    free(tempCmd);
}