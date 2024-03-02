#define CWD_MAX_SIZE 1024

// cwd buffer, given an arbitrary but generous size - good enough for now
extern char cwdenv[CWD_MAX_SIZE + 4];
extern char* cwd;