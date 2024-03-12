
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

#include "../headers/builtins.h"

#include "../headers/args.h"
#include "../headers/config.h"
#include "../main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/wait.h>

extern char** environ;

// internal array of builtins. this is not exposed to the outside, and is not supposed to change dynamically
const builtin allbuiltins[] = {
    {"quit", quit},
    {"clr", clear},
    {"dir", dir},
    {"environ", printenviron},
    {"echo", echo},
    {"cd", cd},
    {"help", help}, // unfinished
    // some extra builtins i have added
    {"lpath", lpath},
    {"putenv", penv},
    {"clrenv", cenv},
    {"source", source},
};

const int numbuiltins = sizeof(allbuiltins) / sizeof(builtin); 

const builtin* getbuiltin(int index)
{
    if (index < 0 || index >= numbuiltins) return NULL;
    return allbuiltins + index;
}

int runbuiltin(int index, char* args[MAX_ARGS])
{
    const builtin* target = getbuiltin(index);
    if (target == NULL) return EXIT_SUCCESS; // don't deref a null*
    return (*(target->function)) (args + 1); // the first arg is known already
}

int quit(char* args[MAX_ARGS])
{
    exit(0);
    return EXIT_SUCCESS;
}

int clear(char* args[MAX_ARGS])
{
    system("clear");
    return EXIT_SUCCESS;
}

int dir(char* args[MAX_ARGS])
{
    char cmd[MAX_CMD_LEN] = "ls -al ";
    if (args[0]) strncat(cmd, args[0], MAX_CMD_LEN - 1);
    system(cmd);
    return EXIT_SUCCESS;
}

int printenviron(char* args[MAX_ARGS])
{
    int i = 0;
    while (environ[i]) printf("%s\n", environ[i++]);
    return EXIT_SUCCESS;
}

int echo(char* args[MAX_ARGS])
{
    if (args[0] == NULL) // if we have no arguments, print a blank line, like most echo implementations
    {
        putchar('\n');
        return EXIT_SUCCESS;
    }

    int i = 0;
    while (args[i] && args[i + 1])
    {
        printf("%s ", args[i]);
        i++;
    }

    printf("%s\n", args[i]);
    return EXIT_SUCCESS;
}

int cd(char* args[MAX_ARGS])
{
    char* target = args[0];
    if (target == NULL) // if given no argument
    {
        printf("%s\n", getenv("PWD"));
        return EXIT_SUCCESS;
    }

    int chstat; // attempt to change directory
    if (strcmp(target, "-") == 0)
    {
        chstat = chdir(getenv("OLDPWD")); // go to previous directory if cd -
    } else
    {
        chstat = chdir(target);
    }

    if (chstat == 0)
    {
        // successful, update OLDPWD
        if (setenv("OLDPWD", cwd, 1))
        {
            perror("cd: could not change OLDPWD: ");
            return EXIT_FAILURE;
        }
        // change PWD to point to the new dir
        if (getcwd(cwd, MAX_DIR_LEN) == NULL) // getcwd will change the memory at cwd
        {
            perror("cd: could not change PWD: ");
            return EXIT_FAILURE + 1;
        }
    } else
    {
        fprintf(stderr, "cd: could not change to '%s': ", target);
        perror("");
        return EXIT_FAILURE + 2;
    }
    return EXIT_SUCCESS;
}

int help(char* args[MAX_ARGS])
{
    // not sure of the smartest way to find the manual
    // decided to change directory to the binary location and have a copy there
    // could have stored it on the net
    char* slash = strrchr(shell, '/'); // any absolute pathname should have at least one slash
    *slash = '\0'; // temporarily replace it with a null
    int chstat = chdir(shell); // so we can move there
    *slash = '/';

    if (chstat == -1) // if we couldn't move there, we can stop now
    {
        fprintf(stderr, "help: could not move to '%s': ", shell);
        return EXIT_FAILURE;
    }

    // figure out what the user's pager is
    const char* pager  = getenv("PAGER") ? getenv("PAGER") : DEFAULT_PAGER;

    // perform the fork
    int pid, pstat, retval = EXIT_SUCCESS;
    switch (pid = fork())
    {
        case -1:
            perror("msh: could not fork to pager");
            retval = EXIT_FAILURE + 1;
            break;
        case 0:
            execlp(pager, pager, MAN_NAME, NULL);
            fprintf(stderr, "help: could not exec %s: ", pager);
            perror("");
            exit(EXIT_FAILURE + 2); // kill the child
            break;
    }

    // we can chdir back while we wait
    chstat = chdir(getenv("PWD"));
    waitpid(pid, &pstat, WUNTRACED);
    if (WIFEXITED(pstat)) retval = WEXITSTATUS(pstat);

    if (chstat == -1)
    {
        fprintf(stderr, "help: could not return to '%s': ", getenv("PWD"));
        perror("");
        retval = EXIT_FAILURE + 3;
    }

    return retval;
}

int lpath(char* args[MAX_ARGS])
{
    longpath = !longpath;
    return EXIT_SUCCESS;
}

int penv(char* args[MAX_ARGS])
{
    char* name = args[0], *value = args[1];
    if (!(name && value)) // report an error if missing both args
    {
        fprintf(stderr, "penv: provide 2 arguments\n");
        return EXIT_FAILURE;
    }

    if (setenv(name, value, 1) == -1)
    {
        fprintf(stderr, "penv: could not set %s=%s: ", name, value);
        perror("");
        return EXIT_FAILURE + 1;
    }

    return EXIT_SUCCESS;
}

int cenv(char* args[MAX_ARGS])
{
    char* name = args[0];
    if (name == NULL)
    {
        fprintf(stderr, "cenv: provide 1 argument\n");
        return EXIT_FAILURE;
    }

    if (unsetenv(name) == -1)
    {
        fprintf(stderr, "cenv: could not clear '%s': ", name);
        perror("");
        return EXIT_FAILURE + 1;
    }

    return EXIT_SUCCESS;
}

int source(char* args[MAX_ARGS])
{
    char* filename = args[0];
    if (filename == NULL)
    {
        fprintf(stderr, "source: provide 1 argument\n");
        return EXIT_FAILURE;
    }
    // defer to feval - the builtin args pointer isn't good enough for this
    return !feval(filename, cmdstr, cmdargs);
}