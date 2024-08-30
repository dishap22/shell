#include "headers.h"

int main()
{
    // Initial setup
    int config_status = 1; // set to not done at start
    do {
        config_status = configure();
    } while (config_status != 0);

    // signal handle for sigchld
    signal(SIGCHLD, handle_sigchld);
 
    char *prevCommand = NULL;
    double time = 0;

    while (1)
    {
        config_status = prompt(time, prevCommand);
        time = 0;
        double execution_status = -1; // set as false by default so if there's a valid command in a multi-command, we can set as true and store the whole string
        if (config_status != 0) continue; // Re-try prompt without taking user input if it fails
        
        char input[CHARACTER_MAX];
        if (fgets(input, sizeof(input), stdin) == NULL) 
        {
            printf(ERROR_COLOR "Failed to input" RESET_COLOR);
            continue;
        }

        // Tokenising
        input[strcspn(input, "\n")] = '\0'; // null-terminate input string if newline character is present

        char *tokens[CHARACTER_MAX];
        int background_flag[CHARACTER_MAX];
        char *delims = "&;";
        char *tempInput = strdup(input);
        char *token;
        int i = 0;

        token = strtok(tempInput, delims);
        while (token != NULL)
        {
            tokens[i] = strdup(token);

            // check if background token or not
            size_t tokenLen = strlen(token);
            size_t offset = token - tempInput + tokenLen;

            if (input[offset] != '\0' && strchr(delims, input[offset]) != NULL)
            {
                if (input[offset] == '&') background_flag[i] = 1;
                else background_flag[i] = 0;
            } 
            else 
            {
                background_flag[i] = 0;
            } 
            i++;
            token = strtok(NULL, delims); // get next token
        }

        tokens[i] = NULL; 
        background_flag[i] = -1; 
        if (strstr(input, "log") == NULL) logCommand(input); // if command doesn't have log in it then log it
        //  Finished tokenizing, now executing 
        for (int j = 0; tokens[j] != NULL; j++) 
        {
            double commandDuration = execute_command(tokens[j], background_flag[j]);
            if (commandDuration > 0) 
            {
                time += commandDuration;
            } // execute command only returns time for foreground processes so in main we just need to add time if it's above 0

            free(tokens[j]);
        }
        if (execution_status != 0) 
        {
            free(prevCommand);
            prevCommand = strdup(input);
        }
        free(tempInput);
    }
}