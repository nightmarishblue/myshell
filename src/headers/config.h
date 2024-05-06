// contains the logic behind msh's configurable elements

// data type that represents a user's shell alias in the form input=output
typedef struct alias
{
    char* name; // the string that this alias searches for - on the heap
    int arglen; // the number of arguments that it takes to expand this alias out
    char** expargs; // the arguments that it is expanded into - expargs[0] is on the heap
} alias;


extern alias* aliases;

extern int longpath; // tracks whether or not the shell should display the full path in its prompt

// prints the prompt, taking into account any config that might alter its format
void printprompt();

// push a new alias to the end shell's list, of the form input=output
// return whether or not the operation was successful
// if it was, then there is new heap mem to free later
int pshalias(char* input, char* output);

// free the memory of every alias, useful for shell shutdown;
void freealiases();

// grab the alias at a specific index in the array
const alias* getalias(int index);

// identify an alias by name
// returns its index or -1 if no registered alias has that name
int idalias(char* name);

// deallocate the elements of the alias at the given index and mark it as open for replacement
// return false if the requested alias doesn't exist
// sets the name to NULL
int clralias(int index);

// replace an alias's expansion with something else
// on a failure, return false and leave the alias unchanged
int replalias(int index, char* expans);
