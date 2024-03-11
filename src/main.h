// contains all the primary macros related to environment

#include "./headers/args.h"

#include <stdio.h>

// an arbitrary assumption on the max length of a dir's path
// this system will probably change later
#define MAX_DIR_LEN 1024

// the global argument objects
extern char cmdstr[MAX_CMD_LEN]; // the string used to store each cmd inputted to msh
extern char* cmdargs[MAX_ARGS + 1]; // the array of pointers that points to the start of each word in cmdstr - terminated by NULL

// cwd buffer, given an arbitrary but generous size - good enough for now
extern char cwdenv[MAX_DIR_LEN + 4];
extern char* cwd;

// buffers and pointers for the shell environment variable
extern char shellenv[MAX_DIR_LEN + 6];
extern char* shell; // points to the string contains msh's path

#define MAN_NAME "readme"
#define DEFAULT_PAGER "more"

// msh doesn't have much in the way of options - until now
extern int longpath; // tracks whether or not the shell should display the full path in its prompt

// get a line from an input source and perform the evaluation
// returns false at EOF, when the shell is normally expected to close
int getrunline(FILE* input, char cmdstr[MAX_CMD_LEN], char* cmdargs[MAX_ARGS + 1]);

// evaluate the contents of filename
// return false if filename couldn't be opened
int feval(char* filename, char cmdstr[MAX_CMD_LEN], char* cmdargs[MAX_ARGS + 1]);