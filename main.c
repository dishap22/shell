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
        char input[4096];
        fgets(input, 4096, stdin);
    }
}