// an arbitrary assumption on the max length of a dir's path
// this system will probably change later
#define MAX_DIR_LEN 1024

// cwd buffer, given an arbitrary but generous size - good enough for now
extern char cwdenv[MAX_DIR_LEN + 4];
extern char* cwd;