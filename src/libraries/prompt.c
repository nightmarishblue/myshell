#include "../main.h"

#include <stdio.h>
#include <string.h>

void printprompt()
{
    char* pwd = (longpath) ? cwd : strrchr(cwd, '/'); // the working directory to print
    printf("%s 8> ", pwd);
}