#include "./headers/args.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[argc])
{
    char cmdbuff[MAX_CMD_LEN]; // create a buffer to store inputted commands
    // create an array of strings to hold each parsed arg
    char* cmdargs[MAX_ARGS];
    int cmdargc;
    
    fgets(cmdbuff, MAX_CMD_LEN, stdin);
    cmdargc = splitargs(cmdbuff, cmdargs);

    printf("%i\n", cmdargc);
    for (int i = 0; i < cmdargc; i++)
    {
        printf("%s\n", cmdargs[i]);
    }

    return 0;
    while (1)
    {
        printf("8> ");

        if (feof(stdin)) exit(0);

    }

    return 0;
}