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
    {"alias", aliascmd},
    {"dealias", dealias},
    {"pause", pause_builtin},
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
    quitshell(EXIT_SUCCESS);
    return EXIT_SUCCESS; // never reached
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
            quitshell(EXIT_FAILURE + 2); // kill the child
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

int aliascmd(char* args[MAX_ARGS])
{
    char* input = args[0], *output = args[1];

    if (input == NULL) // no args given, print all aliases
    {
        int i = 0;
        const alias* curr;
        while ((curr = getalias(i++)))
        {
            if (curr->name == NULL) // if this slot is empty, skip it
                continue;

            printf("%s=", curr->name);
            for (int j = 0; j < curr->arglen - 1; j++)
                printf("%s ", curr->expargs[j]);
            printf("%s\n", curr->expargs[curr->arglen - 1]);
        }
        return EXIT_SUCCESS;
    }

    // only one arg - not valid
    if (input && !output)
    {
        fprintf(stderr, "alias: provide 0 or 2 arguments\n");
        return EXIT_FAILURE + 1;
    }

    // warn the user that they probably shouldn't alias dealias
    if (strcmp(input, "dealias") == 0 && (args[2] == NULL || strcmp(args[2], "-y") != 0))
    {
        printf("alias: you probably should not alias delias - use 'alias dealias %s -y' if you think you know better\n", output);
        return EXIT_FAILURE + 2;
    }

    // if the alias already exists, rewrite it
    int id = idalias(input);
    if (id != -1)
    {
        if (!replalias(id, output))
        {
            fprintf(stderr, "alias: could not alter value of alias '%s' - left unchanged\n", input);
            return EXIT_FAILURE + 3;
        }
        return EXIT_SUCCESS;
    }

    return !pshalias(input, output);
}

int dealias(char* args[MAX_ARGS])
{
    char* name = args[0];
    if (name == NULL)
    {
        freealiases();
        return EXIT_SUCCESS;
    }

    int id = idalias(name);
    if (id == -1)
    {
        fprintf(stderr, "dealias: '%s' is not an alias\n", name);
        return EXIT_FAILURE;
    }

    clralias(id);

    return EXIT_FAILURE;
}

// this one is special - it is not handled like other builtins.
// returns the integer that is the cmd to index hist for
int last(char* args[MAX_ARGS])
{
    char* num = args[0];
    int i = 1; // fall back on 1
    if (num) // parse the inputted number if it is set
    {
        char* errptr = NULL;
        errno = 0;
        i = (int) strtol(num, &errptr, 10);
        if (errptr == num || *errptr != '\0')
        {
            fprintf(stderr, "last: '%s' is not an integer\n", num);
            return EXIT_FAILURE * -1;
        }

        if (errno != 0)
        {
            fprintf(stderr, "last: error occurred when coercing '%s' to integer: ", num);
            perror("");
            return (EXIT_FAILURE + 1) * -1;
        }

        if (i < 1 || i > HIST_LEN)
        {
            fprintf(stderr, "last: %d is not between 1 and %d\n", i, HIST_LEN);
            return (EXIT_FAILURE + 2) * -2;
        }
    }

    return i % HIST_LEN;
}

int pause_builtin(char* args[MAX_ARGS])
{
    while (getchar() != '\n')
    {
        if (feof(stdin)) // make sure to check for EOF
            return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
