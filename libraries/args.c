// handle argument processing and built-ins

#include "../headers/args.h"

#include <stdlib.h>
#include <string.h>

// store of each builtin command, terminated with a sentinel for ease of use
const char* const builtins[] = {"quit", "clr", "dir", "environ", ""};

// given a string, evaluate which of the builtins it is
int parsebuiltin(const char cmd[])
{
    int i = 0;
    while (builtins[i][0] != '\0')
    {
        const char* curr = builtins[i]; // builtin we are currently looking at
        if (strcmp(curr, cmd) == 0) return i;
        i++;
    }
    return -1;
}

int splitargs(char cmdstr[MAX_CMD_LEN], char* argarr[MAX_ARGS])
{
    int index = 0; // index to return as length
    argarr[index++] = strtok(cmdstr, ARG_WHITESPACE); // place the first arg into arr
    while (argarr[index++] = strtok(NULL, ARG_WHITESPACE)); // continue until null
    return --index; // make sure to decrement so that we handle the off-by-one
}