// the logic that handles all builtin functionality

#include "args.h"

#define MAX_BUILTIN_NAME_LEN 12

// a struct/type that represents this shell's builtin.
// name is the cmd that a user types for this builtin
// function is a pointer to the function that executes it
typedef struct builtin {
    char name[MAX_BUILTIN_NAME_LEN];
    void (*function) (char* args[MAX_ARGS]);
} builtin;

// return a pointer to the builtin with this index, or NULL if it's outside the range
const builtin* getbuiltin(int index);

// run the builtin with the given index, using the args provided
// the first arg is skipped, since we should know it at compile time
void runbuiltin(int index, char* args[MAX_ARGS]);

// quit the shell
void quit(char* args[MAX_ARGS]);

// clear the screen
void clear(char* args[MAX_ARGS]);

// list the contents of a directory
void dir(char* args[MAX_ARGS]);

// print all env vars
void printenviron(char* args[MAX_ARGS]);

// print every arg on a line
void echo(char* args[MAX_ARGS]);