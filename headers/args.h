#include <stdlib.h>

#define MAX_CMD_LEN 256 // the maximum length of an input command
#define MAX_ARGS 64 // the maximum number of parsed arguments.
#define ARG_WHITESPACE " \n\t\r" // a rudimentary macro that contains whitespace.
#define ARG_QUOTES "\"'`" // the chars that are considered quotes and will group words together

// take a cmd buffer and split it into tokens, placing each into the argarr
// returns the number of args found, which cannot be more than MAX_ARGS
int splitargs(char cmdstr[MAX_CMD_LEN], char* argarr[MAX_ARGS]);

// given a string, evaluate which of the builtins it is
// if it is not a builtin, return -1
int parsebuiltin(const char cmd[]);

// look for >s in the args, and perform the redirection on  target files
// if user attempts multiple >s, the first is applied and the rest ignored
void parseioredirects(int arglen, char* args[arglen]);