#include <stdlib.h>

#define MAX_CMD_LEN 256 // the maximum length of an input command
#define MAX_ARGS 64 // the maximum number of parsed arguments.
#define ARG_WHITESPACE " \n\t" // a rudimentary macro that contains whitespace.

// take a cmd buffer and split it into tokens, placing each into the argarr
// returns the number of args found, which cannot be more than MAX_ARGS
int splitargs(char cmdstr[MAX_CMD_LEN], char* argarr[MAX_ARGS]);