/*
AUTHOR: Niall Bastible Díaz | 22314443 | niall.bastiblediaz2@mail.dcu.ie
ACKNOWLEDGEMENTS
    I understand that the University regards breaches of academic integrity and plagiarism as grave and serious.

    I have read and understood the DCU Academic Integrity and Plagiarism Policy.
    I accept the penalties that may be imposed should I engage in practice or practices that breach this policy.

    I have identified and included the source of all facts, ideas, opinions and viewpoints of others in the assignment references.
    Direct quotations, paraphrasing, discussion of ideas from books, journal articles, internet sources, module text, or any other
    source whatsoever are acknowledged and the sources cited are identified in the assignment references.

    I declare that this material, which I now submit for assessment, is entirely my own work and has not been taken from the work of
    others save and to the extent that such work has been cited and acknowledged within the text of my work.
*/

// handle argument processing and built-ins

#include "../headers/args.h"
#include "../headers/builtins.h"
#include "../headers/io.h"
#include "../headers/config.h"

#include <stdlib.h>
#include <string.h>

int parsebuiltin(const char cmd[])
{
    int i = 0;
    const builtin* curr; // place to store the builtin we're currently looking at
    while ((curr = getbuiltin(i)))
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
    int len = 0; // index to return as length
    char* arg = tokarg(cmdstr, ARG_WHITESPACE, ARG_QUOTES); // store the current arg into a pointer
    while (arg != NULL && len < MAX_ARGS)
    {
        argarr[len++] = arg;
        arg = tokarg(NULL, ARG_WHITESPACE, ARG_QUOTES);
    }
    argarr[len] = NULL;
    return len;
}

int parseioredirects(char* args[MAX_ARGS])
{
    int output = 0;
    int i = 0;
    while (i < MAX_ARGS && args[i] && args[i + 1]) // loop until the second-last figure
    {
        const ioop* operator = getioop(args[i]);
        // ignore non-operators
        if (operator != NULL)
        {
            // assume the next argument is the target file
            redirectio(operator, args[i + 1]);
            // blank out both of these args, they're no longer useful
            args[i][0] = args[i + 1][0] = '\0';
            output = 1; // we have redirected at least one stream
        }
        i++;
    }
    return output;
}

// remove all blank strings from an arg arr
int cleanargs(char* args[MAX_ARGS])
{
    int newlen = 0, i = 0;

    while (i < MAX_ARGS && args[i])
    {
        char *currarg = args[i];
        // ignore this if it's a blank string
        if (currarg[0] != '\0')
        {
            args[newlen++] = currarg; // overwrite the value in the "new array"
        }
        i++;
    }
    args[newlen] = NULL;
    return newlen;
}

// concatenate an array of strings with a NULL terminator into a single, space separated string
// no longer storing this info
// void concatstrs(char dest[], char* srcstrs[MAX_ARGS])
// {
//     int i = 0;
//     while (i < MAX_ARGS && srcstrs[i] && srcstrs[i + 1])
//     {
//         strcat(dest, srcstrs[i]);
//         strcat(dest, " ");
//         i++;
//     }
//     strcat(dest, srcstrs[i]);
// }

int parsebackground(char* cmdargs[MAX_ARGS])
{
    if (*cmdargs == NULL) return 0; // return early if cmdargs is empty
    while (cmdargs[1] != NULL) cmdargs++; // walk to the end of the array
    if (strcmp(*cmdargs, "&") != 0) return 0; // exit if there is no & operator
    *cmdargs = NULL; // remove the & from the args
    return 1;
}

void expandvars(char* cmdargs[MAX_ARGS])
{
    int i = 0;
    char* curr;
    while ((curr = cmdargs[i]) != NULL)
    {
        if (curr[0] == '$')
        {
            char* envval = getenv(curr + 1);
            if (envval != NULL) cmdargs[i] = envval;
        }
        i++;
    }
}

void expandalias(char* cmdargs[MAX_ARGS])
{
    char* cmd = cmdargs[0];
    int aliasid = idalias(cmd);
    if (aliasid == -1)
        return; // exit if this isn't an alias
    
    // otherwise, we must shift up the array by a certain amount
    const alias* expans = getalias(aliasid);
    int i = 0;
    while (cmdargs[i] != NULL) i++;
    
    // if we don't have the space to expand this array, we should exit and avoid an overflow
    if (i + expans->arglen - 1 >= MAX_ARGS)
    {
        fprintf(stderr, "msh: could not expand alias '%s' - too many arguments in command", cmd);
        return;
    }

    // shift up the arguments
    while (i > 0)
    {
        cmdargs[i + expans->arglen - 1] = cmdargs[i];
        i--;
    }
    
    for (i = 0; i < expans->arglen; i++)
        cmdargs[i] = expans->expargs[i]; // alter the command
}