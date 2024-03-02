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

char cwdenv[MAX_DIR_LEN + 4] = "PWD=";
char* cwd = cwdenv + 4;

int main(int argc, char* argv[argc])
{
    // environment variables
    char exepath[MAX_DIR_LEN + 6] = "SHELL="; // buffer for the path to our executable
    exepath[readlink("/proc/self/exe", exepath + 6, MAX_DIR_LEN) + 6] = '\0'; // remember to terminate, since readlink does not
    putenv(exepath); // set SHELL to our path

    // argument data
    char cmdstr[MAX_CMD_LEN], argbuff[MAX_CMD_LEN]; // create a buffer to store inputted commands
    // create an array of strings to hold each parsed arg, plus the NULL at the end
    char* cmdargs[MAX_ARGS + 1];
    int cmdargc;
    
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
        char* s = fgets(cmdstr, MAX_CMD_LEN, input); // grab a line from stdin
        strncpy(argbuff, cmdstr, MAX_CMD_LEN); // cmdstr holds the unbroken string
        // split that line into args - argbuff holds the strings in cmdargs
        splitargs(argbuff, cmdargs);

        // 3. make sure the user hasn't closed the shell or put in an empty string
        if (feof(input)) exit(0);
        // if (cmdargc == 0) continue; // stop if we somehow have no args

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
                int redirectedio = parseioredirects(cmdargs);
                if (redirectedio) // remove all the redirections from the cmdstring
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

                execvp(cmdargs[0], cmdargs); // replace the process with the desired program
                perror("msh: could not exec"); // this is only reached on error
                break;
            default:
                waitpid(pid, &status, WUNTRACED);
                break;
        }

        // pid_t pid = fork();

        // if (pid == -1)
        // {
        //     fprintf(stderr, "msh: could not fork process: ");
        //     perror("");
        //     exit(errno);
        // }

        // if (pid == 0)
        // {
        //     // in child, execute
        //     int builtin = parsebuiltin(cmdargs[0]); // if this cmd is a builtin, find which one it is
        //     // TODO make the execution of these commands fork and use exec
        //     if (builtin == -1)
        //     {
        //         system(cmdstr); // execute the system command with the entire string.
        //     } else
        //     {
        //         runbuiltin(builtin, cmdargs); // execute the builtin
        //     }
        // }

        // ? + 1. fix the stdout if we changed it
        // if (redirectedio) restoreio();
    }

    return 0;
}