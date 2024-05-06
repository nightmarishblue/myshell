// the logic that handles all builtin functionality
// builtins return the same exit status that programs do

#include "args.h"

#define MAX_BUILTIN_NAME_LEN 12

// a struct/type that represents this shell's builtin.
// name is the cmd that a user types for this builtin
// function is a pointer to the function that executes it
typedef struct builtin {
    char name[MAX_BUILTIN_NAME_LEN];
    int (*function) (char* args[MAX_ARGS]);
} builtin;

// return a pointer to the builtin with this index, or NULL if it's outside the range
const builtin* getbuiltin(int index);

// run the builtin with the given index, using the args provided
// the first arg is skipped, since we should know it at compile time
int runbuiltin(int index, char* args[MAX_ARGS]);

// quit the shell
int quit(char* args[MAX_ARGS]);

// clear the screen
int clear(char* args[MAX_ARGS]);

// list the contents of a directory
int dir(char* args[MAX_ARGS]);

// print all env vars
int printenviron(char* args[MAX_ARGS]);

// print every arg on a line
int echo(char* args[MAX_ARGS]);

// change to the given directory
int cd(char* args[MAX_ARGS]);

// get help on how to use msh
// opens the user's pager to show the manual
int help(char* args[MAX_ARGS]);

// toggle the long path in msh's prompt
int lpath(char* args[MAX_ARGS]);

// set the environment variable args[0]=args[1]
int penv(char* args[MAX_ARGS]);

// clear the environment variable with the name args[0]
int cenv(char* args[MAX_ARGS]);

// evaluate the contents of the file with name args[0]
int source(char* args[MAX_ARGS]);

// alias the command arg[1] to the shortcut arg[0]
// print all current aliases if no args given
int aliascmd(char* args[MAX_ARGS]);

// remove the alias at arg[0]
int dealias(char* args[MAX_ARGS]);

// run the command X commands ago - last 1 runs the previous command
// this one is SPECIAL - it is not handled like other builtins.
// returns the number of elements to index backwards from the current length of hist
// on a failure, returns negative
int last(char* args[MAX_ARGS]);

// pause the shell unter the user hits enter
int pause_builtin(char* args[MAX_ARGS]);
