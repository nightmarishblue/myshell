/*
AUTHOR: Niall Bastible Díaz | 22314443 | niall.bastiblediaz2@mail.dcu.ie
ACKNOWLEDGEMENTS
    I understand that the University regards breaches of academic integrity and plagiarism as grave and serious.

    I have read and understood the DCU Academic Integrity and Plagiarism Policy.
    I accept the penalties that may be imposed should I engage in practice or practices that breach this policy.

    I have identified and included the source of all facts, ideas, opinions and viewpoints of others in the assignment references.
    Direct quotations, paraphrasing, discussion of ideas from books, journal articles, internet sources, module text, or any other
    source whatsoever are acknowledged and the sources cited are identified in the assignment references.

    I declare that this material, which I now submit for assessment, is entirely my own work and has not been taken from the work of
    others save and to the extent that such work has been cited and acknowledged within the text of my work.
*/

// contains all the primary macros related to environment

#include "./headers/args.h"

#include <stdio.h>

#define HIST_LEN 16 // the number of commands that are stored in the shell's history.
extern char cmdhist[HIST_LEN][MAX_CMD_LEN]; // the commands that are stored in msh's history

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

#define RC_NAME ".mshrc"

// get a line from an input source and perform the evaluation
// returns false at EOF, when the shell is normally expected to close
int getrunline(FILE* input, char cmdstr[MAX_CMD_LEN], char* cmdargs[MAX_ARGS + 1]);

// evaluate the contents of filename
// return false if filename couldn't be opened
int feval(char* filename, char cmdstr[MAX_CMD_LEN], char* cmdargs[MAX_ARGS + 1]);

// perform necessary cleanup and quit the shell
void quitshell(int status);

// push a command into the shell's history
// it will go somewhere in a circular array, and the array's pointer will go up
void pshhist(char* cmd);

// index the history to return a certain command - wraps around the circular buffer
// returns a blank if the index requested has no command stored in it
char* gethist(int index);