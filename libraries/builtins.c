
#include "../headers/builtins.h"

#include "../headers/args.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <unistd.h>

extern char** environ;

// exit the shell
void quit(int arglen, char* args[arglen])
{
    exit(0);
}

// clear the screen
void clear(int arglen, char* args[arglen])
{
    system("clear");
}

// list the contents of a directory
void dir(int arglen, char* args[arglen])
{
    char cmd[MAX_CMD_LEN] = "ls -al ";
    if (arglen > 1) strncat(cmd, args[1], MAX_CMD_LEN - 1);
    system(cmd);
}

// print all env vars
void printenviron(int arglen, char* args[arglen])
{
    int i = 0;
    while (environ[i]) printf("%s\n", environ[i++]);
}