#include "./headers/args.h"
#include "./headers/builtins.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[argc])
{
    char cmdbuff[MAX_CMD_LEN]; // create a buffer to store inputted commands
    // create an array of strings to hold each parsed arg
    char* cmdargs[MAX_ARGS];
    int cmdargc;
    
    while (1)
    {
        printf("8> ");

        char* s = fgets(cmdbuff, MAX_CMD_LEN, stdin); // grab a line from stdin
        cmdargc = splitargs(cmdbuff, cmdargs); // split that line into args

        // printf("%i %i\n", s, cmdargc);

        if (feof(stdin)) exit(0);
        if (cmdargc == 0) continue; // stop if we somehow have no args

        int builtin = parsebuiltin(cmdargs[0]); // if this cmd is a builtin, find which one it is
        if (builtin == -1)
        {
            system(cmdbuff); // execute the system command with the entire string.
            continue;
        }
        runbuiltin(builtin, cmdargc, cmdargs); // execute the builtin
    }

    return 0;
}