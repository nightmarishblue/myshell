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