// handle argument processing and built-ins

#include "../headers/args.h"
#include "../headers/builtins.h"

#include <stdlib.h>
#include <string.h>

int parsebuiltin(const char cmd[])
{
    int i = 0;
    const builtin* curr; // place to store the builtin we're currently looking at
    while (curr = getbuiltin(i))
    {
        if (strcmp(curr->name, cmd) == 0) return i; // check if this builtin's name is our cmd
        i++;
    }
    return -1;
}

int splitargs(char cmdstr[MAX_CMD_LEN], char* argarr[MAX_ARGS])
{
    int index = 0; // index to return as length
    char* arg = strtok(cmdstr, ARG_WHITESPACE); // store the current arg into a pointer
    while (arg != NULL && index < MAX_ARGS)
    {
        argarr[index++] = arg;
        arg = strtok(NULL, ARG_WHITESPACE);
    }
    return index;
}