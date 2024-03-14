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

#include "main.h"

#include "./headers/args.h"
#include "./headers/builtins.h"
#include "./headers/io.h"
#include "./headers/config.h"

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
char cmdhist[HIST_LEN][MAX_CMD_LEN] = {0};
int histlen = 0; // an index for the circular array

char cwdenv[MAX_DIR_LEN + 4] = "PWD=";
char* cwd = cwdenv + 4;

char shellenv[MAX_DIR_LEN + 6] = "SHELL=";
char* shell = shellenv + 6;

char parentenv[MAX_DIR_LEN + 7] = "PARENT=";

char manloc[MAX_DIR_LEN + 7];

void reapzombies(); // helper function to reap any shambling children and print their exit status

int main(int argc, char* argv[argc])
{
    // environment variables
    // TODO: check this value for the size being too big
    shellenv[readlink("/proc/self/exe", shell, MAX_DIR_LEN) + 6] = '\0'; // remember to terminate, since readlink does not
    strncpy(parentenv + 7, shell, MAX_DIR_LEN - strlen(parentenv)); // also copy this to the parentenv
    putenv(shellenv); // set SHELL to our path

    // get the manual location
    strncpy(manloc, shell, MAX_DIR_LEN);
    strcpy(strrchr(manloc, '/') + 1, MAN_NAME); // we will chop off the filename and replace it with readme
    // aware of the danger here.

    if (getcwd(cwd, MAX_DIR_LEN) == NULL || putenv(cwdenv) != 0)
    {
        perror("msh: could not correctly set PWD: ");
        quitshell(EXIT_FAILURE); // should exit for safety
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
            quitshell(EXIT_FAILURE);
    }
    else
    {
        do
        {
            reapzombies();
            printprompt();
        } while (getrunline(stdin, cmdstr, cmdargs));
    }

    return 0;
}

int getrunline(FILE* input, char cmdstr[MAX_CMD_LEN], char* cmdargs[MAX_ARGS + 1])
{
    // reap any existing zombies
    reapzombies();
    // the 5 steps of the shell's loop
    // 1. get input and then split it into lines
    fgets(cmdstr, MAX_CMD_LEN, input);     // grab a line from stdin
    
    // 1.5. handle the last builtin - it is weird
    if (strcspn(cmdstr, ARG_WHITESPACE) == 4 && strncmp(cmdstr, "last", 4) == 0)
    {
        splitargs(cmdstr, cmdargs); // perhaps could split only the first two arguments 
        int i = last(cmdargs + 1);
        if (i < 0)
            return 1;
        char* hist = gethist(histlen - i);
        strncpy(cmdstr, hist, MAX_CMD_LEN);
    } else
    {
        pshhist(cmdstr); // save this command in the history
    }

    // split that line into args
    splitargs(cmdstr, cmdargs);

    // 2. make sure the user hasn't closed the shell or put in an empty string
    if (feof(input))
        return 0;
    if (cmdargs[0] == NULL)
        return 1; // stop if we somehow have no args

    // 4. check if the last arg is & - we should background process if it is, and wipe it from the list
    int shouldwait = !parsebackground(cmdargs);
    expandalias(cmdargs);

    // 5A. figure out whether the command is a builtin - if so, we shouldn't fork
    int builtin = parsebuiltin(cmdargs[0]);
    if (builtin != -1)
    {
        // redirect this process's output
        if (parseioredirects(cmdargs))
            cleanargs(cmdargs); // remove all the redirections from the cmd args

        expandvars(cmdargs);
        runbuiltin(builtin, cmdargs);

        // set $_ to the last arg
        int arglen = 0;
        while (cmdargs[arglen]) arglen++;
        setenv("_", cmdargs[arglen - 1], 1);

        restoreio(); // restore the io, since this is still our parent process
        return 1;
    }

    // 5B. else fork the program and try to serve the command
    int pid, status;
    switch (pid = fork())
    {
    case -1:
        perror("msh: could not fork process: ");
        // quitshell(errno); // this is not a breaking error that requires the shell to stop
        break;
    case 0: // fork was successful, this is the child
        // if we shouldn't wait, then this goes to the background pg
        if (!shouldwait)
            setpgid(0, 0);

        // check if the user is redirecting - we need not restore the io state as the child will die
        if (parseioredirects(cmdargs))
            cleanargs(cmdargs);

        expandvars(cmdargs);
        putenv(parentenv); // ensure the child has the correct parent

        execvp(cmdargs[0], cmdargs); // replace the process with the desired program
        fprintf(stderr, "msh: could not exec '%s': ", cmdargs[0]);
        perror("");             // this is only reached on error
        quitshell(EXIT_FAILURE); // make sure we exit in that case
        break;
    }

    // don't forget to set $_ - we could have used a pipe but this requires less i/o
    // also considered "queueing" I/O operations so that we know what cmdargs looks like in the parent
    // either way, we have to wait on the child anyway so we may as well do it here
    int arglen = 0;
    while (cmdargs[arglen] && cmdargs[arglen + 1]) arglen++;
    while (arglen > 0 && getioop(cmdargs[arglen - 1]))
    {
        arglen -= 2;
    }
    if (arglen < 0)
        arglen = 0;
    setenv("_", cmdargs[arglen], 1);

    if (shouldwait)
    {
        waitpid(pid, &status, WUNTRACED);
    }
    else
    {
        printf("+& %d\n", pid);
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
    do
    {
        reapzombies();
    }
    while (getrunline(input, cmdstr, cmdargs));
    fclose(input);
    return 1;
}

void quitshell(int status)
{
    freealiases();
    exit(status);
}

void reapzombies()
{
    int pid, status;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        printf("-& %d (%d)\n", pid, status);
    }
}

void pshhist(char* cmd)
{
    char* histslot = cmdhist[histlen++]; // grab the slot to store this cmd in (and increment len)
    strncpy(histslot, cmd, MAX_CMD_LEN); // copy it there
    histlen %= HIST_LEN; // clamp it back to the array's range
}

char* gethist(int index)
{
    if (index < 0)
        index = HIST_LEN - ((index * -1) % HIST_LEN); // C's modulo operator does not floor divide or something
    return cmdhist[index % HIST_LEN];
}