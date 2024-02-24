#include "../headers/args.h"

#include <stdlib.h>
#include <string.h>

size_t splitargs(char cmdstr[MAX_CMD_LEN], char* argarr[MAX_ARGS])
{
    size_t index = 0; // index to return as length
    argarr[index++] = strtok(cmdstr, ARG_WHITESPACE); // place the first arg into arr
    while (argarr[index++] = strtok(NULL, ARG_WHITESPACE)); // continue until null
    return --index; // make sure to decrement so that we handle the off-by-one
}