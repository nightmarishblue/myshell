#include "./headers/args.h"
#include "./headers/builtins.h"
#include "./headers/io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[argc])
{
    // argument data
    char cmdbuff[MAX_CMD_LEN], tmpbuff[MAX_CMD_LEN]; // create a buffer to store inputted commands
    // create an array of strings to hold each parsed arg
    char* cmdargs[MAX_ARGS];
    int cmdargc;
    
    while (1)
    {
        // 1. print the prompt
        printf("8> ");

        // 2. wait for the user to type something, and split it into a list of args
        char* s = fgets(cmdbuff, MAX_CMD_LEN, stdin); // grab a line from stdin
        strncpy(tmpbuff, cmdbuff, MAX_CMD_LEN);
        cmdargc = splitargs(tmpbuff, cmdargs); // split that line into args

        // 3. make sure the user hasn't closed the shell or put in an empty string
        if (feof(stdin)) exit(0);
        if (cmdargc == 0) continue; // stop if we somehow have no args

        // 4. check if user is redirecting i/o and accomodate
        parseioredirects(cmdargc, cmdargs);

        // ?. execute the command
        int builtin = parsebuiltin(cmdargs[0]); // if this cmd is a builtin, find which one it is
        if (builtin == -1)
        {
            system(cmdbuff); // execute the system command with the entire string.
        } else
        {
            runbuiltin(builtin, cmdargc, cmdargs); // execute the builtin
        }

        // ? + 1. fix the stdout if we changed it
        restoreio();
    }

    return 0;
}