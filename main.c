#include "headers.h"

int main()
{
    // Initial setup
    int config_status = 1; // set to not done at start
    do {
        config_status = configure();
    } while (config_status != 0);
 
    while (1)
    {
        config_status = prompt();
        if (config_status != 0) continue; // Re-try prompt without taking user input if it fails
        
        char input[CHARACTER_MAX];
        // TODO: error message formatting here too
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Failed to input");
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

            size_t tokenLen = strlen(token);
            size_t offset = token - tempInput + tokenLen;

            if (input[offset] != '\0' && strchr(delims, input[offset]) != NULL) {
                if (input[offset] == '&') background_flag[i] = 1;
                else background_flag[i] = 0;
            } else {
                background_flag[i] = 0;
            } 
            i++;
            token = strtok(NULL, delims);
        }
        tokens[i] = NULL; 
        background_flag[i] = -1; 

        //  Finished tokenizing, now cleaning spaces/tabs and executing 
        for (int j = 0; tokens[j] != NULL; j++) {
            // Remove trailing and leading whitespaces and tabs
            char *cmd = tokens[j];
            while (*cmd == ' ' || *cmd == '\t') cmd++;
            char *end = cmd + strlen(cmd) - 1;
            while (end > cmd && (*end == ' ' || *end == '\t')) end--;
            *(end + 1) = '\0';

            // TODO: implement execution & print error if erroneous
            execute_command(cmd, background_flag[j]);
            // printf("Token: '%s' Background flag: %d\n", cmd, background_flag[j]);
            free(tokens[j]);
        }
        free(tempInput);
    }
}