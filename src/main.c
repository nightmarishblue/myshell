#include "main.h"

#include "./headers/args.h"
#include "./headers/builtins.h"
#include "./headers/io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/wait.h>

// the external chars that are used by various parts of msh

char cwdenv[MAX_DIR_LEN + 4] = "PWD=";
char* cwd = cwdenv + 4;

char shellenv[MAX_DIR_LEN + 6] = "SHELL=";
char* shell = shellenv + 6;

char parentenv[MAX_DIR_LEN + 7] = "PARENT=";

int main(int argc, char* argv[argc])
{
    // environment variables
    shellenv[readlink("/proc/self/exe", shell, MAX_DIR_LEN) + 6] = '\0'; // remember to terminate, since readlink does not
    strcpy(parentenv + 7, shell); // also copy this to the parentenv
    putenv(shellenv); // set SHELL to our path

    if (getcwd(cwd, MAX_DIR_LEN) == NULL || putenv(cwdenv) != 0)
    {
        perror("msh: could not correctly set PWD: ");
        exit(errno); // should exit for safety
    }

    // argument data
    char cmdstr[MAX_CMD_LEN], argbuff[MAX_CMD_LEN]; // create a buffer to store inputted commands
    // create an array of strings to hold each parsed arg, plus the NULL at the end
    char* cmdargs[MAX_ARGS + 1];
    
    FILE* input; // the iostream to pull our commands from
    int interactive = 1;
    if (argc > 1) // we have a file to run
    {
        interactive = 0;
        char* filename = argv[1];
        input = fopen(filename, "r");
        if (input == NULL)
        {
            fprintf(stderr, "msh: could not open file '%s': ", filename);
            perror("");
            exit(errno);
        }
    } else
    {
        input = stdin;
    }

    while (1)
    {
        // 1. print the prompt
        if (interactive) printf("%s 8> ", strrchr(getenv("PWD"), '/'));

        // 2. wait for the user to type something, and split it into a list of args
        fgets(cmdstr, MAX_CMD_LEN, input); // grab a line from stdin
        strncpy(argbuff, cmdstr, MAX_CMD_LEN); // cmdstr holds the unbroken string
        // split that line into args - argbuff holds the strings in cmdargs
        splitargs(argbuff, cmdargs);

        // 3. make sure the user hasn't closed the shell or put in an empty string
        if (feof(input)) exit(0);
        if (cmdargs[0] == NULL) continue; // stop if we somehow have no args

        // 3.5 check if the last arg is & - we should background process if it is, and wipe the last arg
        int shouldwait = !parsebackground(cmdargs);

        // 4. fork the program and try to serve the command
        int pid, status;
        switch (pid = fork())
        {
            case -1:
                // fprintf(stderr, "msh: could not fork process: ");
                perror("msh: could not fork");
                // exit(errno);
                break;
            case 0: // fork was successful, this is the child
                // 5. check if user is redirecting i/o and accomodate
                if (parseioredirects(cmdargs)) // remove all the redirections from the cmdstring
                {
                    cleanargs(cmdargs);
                    concatstrs(cmdstr, cmdargs);
                }

                // 6. figure out whether the command is a builtin and execute accordingly
                int builtin = parsebuiltin(cmdargs[0]); // if this cmd is a builtin, find which one it is
                if (builtin != -1)
                {
                    runbuiltin(builtin, cmdargs); // run the builtin with our args and exit this copy after
                    return 0;
                }

                putenv(parentenv); // ensure the child has the correct parent
                execvp(cmdargs[0], cmdargs); // replace the process with the desired program
                perror("msh: could not exec"); // this is only reached on error
                break;
            default:
                if (shouldwait) waitpid(pid, &status, WUNTRACED);
                break;
        }
    }

    return 0;
}