// handle argument processing and built-ins

#include "../headers/args.h"
#include "../headers/builtins.h"
#include "../headers/io.h"

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

// alternative to strtok that takes quotes into account.
char* argindexpointer = NULL;
char* tokarg(char* __restrict__ string, const char* __restrict__ delim, const char* __restrict__ quote)
{
    if (string == NULL)
        string = argindexpointer; // use our stored index like strtok
    if (string == NULL)
        return NULL; // basic error handling

    // if the string starts with delim, we need to ignore it and move up
    string += strspn(string, delim);
    char first = string[0];

    if (first == '\0') // handle the string being empty
    {
        argindexpointer = NULL;
        return NULL;
    }

    // if first is a quote; we need to group it up with its pair
    if (strchr(quote, first) != NULL) // check if this is a quote
    {
        char* endquote = strchr(string + 1, first); // find the next quote in the string
        if (endquote != NULL) // if it exists, we have a match
        {
            *endquote = '\0'; // terminate the endquote
            argindexpointer = endquote + 1; // set our index pointer to the next char
            return string + 1; // return the pointer; this will make a string that contains the quoted text
        }
    }
    
    // case 3: normal char, just split to the next space
    char* whitespace = strpbrk(string, delim);
    if (whitespace == NULL || *whitespace == '\0') // if this is a null, then we are at the end of the string
    {
        argindexpointer = NULL;
    } else
    {
        *whitespace = '\0';
        argindexpointer = whitespace + 1;
    }
    return string;
}


int splitargs(char cmdstr[MAX_CMD_LEN], char* argarr[MAX_ARGS])
{
    int index = 0; // index to return as length
    char* arg = tokarg(cmdstr, ARG_WHITESPACE, ARG_QUOTES); // store the current arg into a pointer
    while (arg != NULL && index < MAX_ARGS)
    {
        argarr[index++] = arg;
        arg = tokarg(NULL, ARG_WHITESPACE, ARG_QUOTES);
    }
    return index;
}

void parseioredirects(int arglen, char* args[arglen])
{
    for (int i = 0; i < arglen - 1; i++) // loop until the second-last figure
    {
        const ioop* operator = getioop(args[i]);
        if (operator == NULL) continue; // ignore non-operators
        // assume the next argument is the target file
        redirectio(operator, args[i + 1]);
    }
}