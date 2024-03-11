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

#include "../headers/io.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

// array of io operations that are valid in our shell, along with the fd no and modes
const ioop allioops[] = {
    {">", "w", STDOUT_FILENO},
    {">>", "a", STDOUT_FILENO},
    {"2>", "w", STDERR_FILENO},
    {"2>>", "a", STDERR_FILENO},
    {"<", "r", STDIN_FILENO},
};

const int numioops = sizeof(allioops) / sizeof(ioop);

const ioop* getioop(char* symbol)
{
    for (int i = 0; i < numioops; i++)
    {
        const ioop* curr = allioops + i;
        if (strcmp(curr->symbol, symbol) == 0) return curr;
    }
    return NULL;
}

FILE* getiofd(int ionum) // helper function to find the stdio FILE* of an integer
{
    switch (ionum)
    {
        case 0:
            return stdin;
            break;
        case 1:
            return stdout;
            break;
        case 2:
            return stderr;
            break;
        default:
            return NULL;
            break;
    }
}

int siobaks[3] = {-1, -1, -1}; // integers to back up our primary fds when we dup - 0, 1, 2

void redirectio(const ioop* op, char* filename)
{
    int fdno = op->fdno;
    if (siobaks[fdno] != -1) return; // do not try to redirect a stream twice
    FILE* iofd = getiofd(fdno);
    if (iofd == NULL) return; // an invalid fdno is cause to exit early

    // try to open our target file using the correct flags
    FILE* tarfd = fopen(filename, op->mode);
    if (tarfd == NULL) // give feedback if there is an error opening file
    {
        fprintf(stderr, "msh: error opening file '%s': ", filename);
        perror("");
        return;
    }

    // otherwise we can flush the fd and start duping
    fflush(iofd);
    siobaks[fdno] = dup(fdno);
    dup2(fileno(tarfd), fdno);
    fclose(tarfd);
}

void restoreio()
{
    for (int siono = 0; siono < 3; siono++)
    {
        if (siobaks[siono] != -1) // check if this has been changed
        {
            int fdno = siobaks[siono]; // the fdno that has been assigned
            FILE* iofd = getiofd(siono); // grab the FILE* that has been reset
            fflush(iofd);
            dup2(fdno, siono);
            close(fdno);
            siobaks[siono] = -1;
        };
    }
}