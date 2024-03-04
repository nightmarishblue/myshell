
#include "../headers/builtins.h"

#include "../headers/args.h"
#include "../main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern char** environ;

// internal array of builtins. this is not exposed to the outside, and is not supposed to change dynamically
const builtin allbuiltins[] = {
    {"quit", quit},
    {"clr", clear},
    {"dir", dir},
    {"environ", printenviron},
    {"echo", echo},
    {"cd", cd},
};

const int numbuiltins = sizeof(allbuiltins) / sizeof(builtin); 

const builtin* getbuiltin(int index)
{
    if (index < 0 || index >= numbuiltins) return NULL;
    return allbuiltins + index;
}

void runbuiltin(int index, char* args[MAX_ARGS])
{
    const builtin* target = getbuiltin(index);
    if (target == NULL) return; // don't deref a null*
    (*(target->function)) (args + 1); // the first arg is known already
}

void quit(char* args[MAX_ARGS])
{
    exit(0);
}

void clear(char* args[MAX_ARGS])
{
    system("clear");
}

void dir(char* args[MAX_ARGS])
{
    char cmd[MAX_CMD_LEN] = "ls -al ";
    if (args[0]) strncat(cmd, args[0], MAX_CMD_LEN - 1);
    system(cmd);
}

void printenviron(char* args[MAX_ARGS])
{
    int i = 0;
    while (environ[i]) printf("%s\n", environ[i++]);
}

void echo(char* args[MAX_ARGS])
{
    if (args[0] == NULL) // if we have no arguments, print a blank line, like most echo implementations
    {
        putchar('\n');
        return;
    }
    int i = 0;
    while (args[i] && args[i + 1])
    {
        printf("%s ", args[i]);
        i++;
    }
    printf("%s\n", args[i]);
}

void cd(char* args[MAX_ARGS])
{
    if (!args[0]) // if given no argument
    {
        printf("%s\n", getenv("PWD"));
        return;
    }

    if (chdir(args[0]) == 0) // attempt to change directory
    {
        // successful, change PWD
        getcwd(cwd, MAX_DIR_LEN);
        printf("%s\n", cwdenv);
        putenv(cwdenv);
    } else
    {
        fprintf(stderr, "cd: could not change to '%s': ", args[0]);
        perror("");
    }
}