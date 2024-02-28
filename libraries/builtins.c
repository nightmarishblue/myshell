
#include "../headers/builtins.h"

#include "../headers/args.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <unistd.h>

extern char** environ;

// internal array of builtins. this is not exposed to the outside, and is not supposed to change dynamically
const builtin allbuiltins[] = {
    {"quit", quit},
    {"clr", clear},
    {"dir", dir},
    {"environ", printenviron},
    {"echo", echo},
};

const int numbuiltins = sizeof(allbuiltins) / sizeof(builtin); 

const builtin* getbuiltin(int index)
{
    if (index < 0 || index >= numbuiltins) return NULL;
    return allbuiltins + index;
}

void runbuiltin(int index, int arglen, char* args[arglen])
{
    const builtin* target = getbuiltin(index);
    if (target == NULL) return; // don't deref a null*
    (*(target->function)) (arglen - 1, args + 1); // the first arg is known already
}

void quit(int arglen, char* args[arglen])
{
    exit(0);
}

void clear(int arglen, char* args[arglen])
{
    system("clear");
}

void dir(int arglen, char* args[arglen])
{
    char cmd[MAX_CMD_LEN] = "ls -al ";
    if (arglen != 0) strncat(cmd, args[0], MAX_CMD_LEN - 1);
    system(cmd);
}

void printenviron(int arglen, char* args[arglen])
{
    int i = 0;
    while (environ[i]) printf("%s\n", environ[i++]);
}

void echo(int arglen, char* args[arglen])
{
    for (int i = 0; i < arglen - 1; i++)
    {
        printf("%s ", args[i]);
    }
    printf("%s\n", args[arglen - 1]);
}