#include "./headers/args.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[argc])
{
    char buff[MAX_CMD_LEN] = "the quick brown fox jumped over the lazy dog"; // create a buffer to store inputted commands
    // create an array of strings to hold each parsed arg
    char* cmdargs[MAX_ARGS];
    size_t cmdargc = splitargs(buff, cmdargs);
    printf("%zi\n", cmdargc);
    for (size_t i = 0; i < cmdargc; i++) {
        printf("%s\n", cmdargs[i]);
    }
    return 0;
}