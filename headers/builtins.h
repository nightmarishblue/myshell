// the logic that handles all builtin functionality

#define MAX_BUILTIN_NAME_LEN 12

// a struct/type that represents this shell's builtin.
// name is the cmd that a user types for this builtin
// function is a pointer to the function that executes it
typedef struct builtin {
    char name[MAX_BUILTIN_NAME_LEN];
    void (*function) (int arglen, char* args[arglen]);
} builtin;

// return a pointer to the builtin with this index, or NULL if it's outside the range
const builtin* getbuiltin(int index);

// run the builtin with the given index, using the args provided
void runbuiltin(int index, int arglen, char* args[arglen]);

void quit(int arglen, char* args[arglen]);

// clear the screen
void clear(int arglen, char* args[arglen]);

// list the contents of a directory
void dir(int arglen, char* args[arglen]);

// print all env vars
void printenviron(int arglen, char* args[arglen]);

// print every arg on a line
void echo(int arglen, char* args[arglen]);