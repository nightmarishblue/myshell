#include "main.h"

#include "./headers/args.h"
#include "./headers/builtins.h"
#include "./headers/io.h"
#include "./headers/prompt.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/wait.h>

// the external chars that are used by various parts of msh

char cmdstr[MAX_CMD_LEN];
char* cmdargs[MAX_ARGS + 1];

char cwdenv[MAX_DIR_LEN + 4] = "PWD=";
char* cwd = cwdenv + 4;

char shellenv[MAX_DIR_LEN + 6] = "SHELL=";
char* shell = shellenv + 6;

char parentenv[MAX_DIR_LEN + 7] = "PARENT=";

char manloc[MAX_DIR_LEN + 7];

// the shell options
int longpath = 1;

int main(int argc, char* argv[argc])
{
    // environment variables
    // TODO: check this value for the size being too big
    shellenv[readlink("/proc/self/exe", shell, MAX_DIR_LEN) + 6] = '\0'; // remember to terminate, since readlink does not
    strcpy(parentenv + 7, shell); // also copy this to the parentenv
    putenv(shellenv); // set SHELL to our path

    // get the manual location
    strcpy(manloc, shell);
    strcpy(strrchr(manloc, '/') + 1, MAN_NAME); // we will chop off the filename and replace it with readme
    // aware of the danger here.

    if (getcwd(cwd, MAX_DIR_LEN) == NULL || putenv(cwdenv) != 0)
    {
        perror("msh: could not correctly set PWD: ");
        exit(EXIT_FAILURE); // should exit for safety
    }

    // try to source the user's ~/.mshrc
    char rcfile[MAX_DIR_LEN];
    if (getenv("HOME"))
    {
        strncpy(rcfile, getenv("HOME"), MAX_DIR_LEN);
        strncat(rcfile, "/", MAX_DIR_LEN - strlen(rcfile));
        strncat(rcfile, RC_NAME, MAX_DIR_LEN - strlen(rcfile));
        if (access(rcfile, F_OK) == 0) // source this file if it exists
            feval(rcfile, cmdstr, cmdargs);
    }

    // now the commands can actually start being processed
    if (argc > 1) // we have a file to run
    {
        if (!feval(argv[1], cmdstr, cmdargs))
            exit(EXIT_FAILURE);
    }
    else
    {
        do {
                printprompt();
        } while (getrunline(stdin, cmdstr, cmdargs));
    }

    return 0;
}

int getrunline(FILE* input, char cmdstr[MAX_CMD_LEN], char* cmdargs[MAX_ARGS + 1])
{
    // the 5 steps of the shell's loop
    // 1. get input and then split it into lines
    fgets(cmdstr, MAX_CMD_LEN, input);     // grab a line from stdin
    // split that line into args
    splitargs(cmdstr, cmdargs);

    // 2. make sure the user hasn't closed the shell or put in an empty string
    if (feof(input))
        return 0;
    if (cmdargs[0] == NULL)
        return 1; // stop if we somehow have no args

    // 4. check if the last arg is & - we should background process if it is, and wipe it from the list
    int shouldwait = !parsebackground(cmdargs);

    // 5A. figure out whether the command is a builtin - if so, we shouldn't fork
    int builtin = parsebuiltin(cmdargs[0]);
    if (builtin != -1)
    {
        // redirect this process's output
        if (parseioredirects(cmdargs))
            cleanargs(cmdargs); // remove all the redirections from the cmd args

        expandvars(cmdargs);
        runbuiltin(builtin, cmdargs);

        restoreio(); // restore the io, since this is still our parent process
        return 1;
    }

    // 5B. else fork the program and try to serve the command
    int pid, status;
    switch (pid = fork())
    {
    case -1:
        perror("msh: could not fork process: ");
        // exit(errno); // this is not a breaking error that requires the shell to stop
        break;
    case 0: // fork was successful, this is the child
        // check if the user is redirecting - we need not restore the io state as the child will die
        if (parseioredirects(cmdargs))
            cleanargs(cmdargs);

        expandvars(cmdargs);
        putenv(parentenv); // ensure the child has the correct parent

        execvp(cmdargs[0], cmdargs); // replace the process with the desired program
        fprintf(stderr, "msh: could not exec '%s': ", cmdargs[0]);
        perror("");             // this is only reached on error
        exit(EXIT_FAILURE + 2); // make sure we exit in that case
        break;
    }

    if (shouldwait)
    {
        waitpid(pid, &status, WUNTRACED);
    }
    else
    {
        printf("& %d\n", pid);
    }

    return 1;
}

int feval(char* filename, char cmdstr[MAX_CMD_LEN], char* cmdargs[MAX_ARGS + 1])
{
    FILE* input = fopen(filename, "r"); // attempt to read file
    if (input == NULL)
    {
        fprintf(stderr, "msh: could not open file '%s': ", filename);
        perror("");
        return 0;
    }
    // attempt to open the file
    while (getrunline(input, cmdstr, cmdargs));
    fclose(input);
    return 1;
}