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

// abstract away the i/o operations

#include <stdio.h>

// io operators take the form ">" and have associated file modes and an fd id num
typedef struct ioop {
    char symbol[4];
    char mode[2];
    int fdno;
} ioop;

// with a symbol, return the associated io operation
// or NULL, if it is not an operator
const ioop* getioop(char* symbol);

// try to perform an io redirection operation on the file with the given location
void redirectio(const ioop* op, char* filename);

// restore any altered streams to their normal state
void restoreio();